/*
*/

#include "GameApi_h.hh"

#include <GL/glew.h>
#include <SDL_opengl.h>

#ifdef VIRTUAL_REALITY
#include "openvr/openvr.h"


class VR {
public:

  struct FramebufferDesc
  {
    GLuint m_nDepthBufferId;
    GLuint m_nRenderTextureId;
    GLuint m_nRenderFramebufferId;
    GLuint m_nResolveTextureId;
    GLuint m_nResolveFramebufferId;
  };

public:
  void InitVR(); // before SDL_CreateWindow
  bool Initialized() const { return hmd!=NULL; }
  void CreateFrameBuffers(); // at init
  void Cleanup(); // at end
  void UpdatePose(); // after rendering
  void Submit(); // at rendering
  void HandleEvent();
  void SetupCameras();
  void RenderStereoTargets(std::function<void()> render_left, std::function<void()> render_right);
  Matrix GetCurrentViewProjectionMatrix( vr::Hmd_Eye nEye );
  void UpdateActionState();
private:
  bool CreateFrameBuffer(int width, int height, FramebufferDesc &desc);
  void Submit(int texid_left, int texid_right);
  static Matrix ConvertMatrix( const vr::HmdMatrix34_t &pose);
  void ProcessVREvent(vr::VREvent_t event);
  Matrix GetHMDMatrixPoseEye( vr::Hmd_Eye nEye );
  Matrix GetHMDMatrixProjectionEye( vr::Hmd_Eye nEye );
private:
  vr::IVRSystem *hmd;
  vr::TrackedDevicePose_t pose[ vr::k_unMaxTrackedDeviceCount ];
  Matrix device_pose[ vr::k_unMaxTrackedDeviceCount ];
  
  Matrix hmd_pose;

  FramebufferDesc leftEyeDesc;
  FramebufferDesc rightEyeDesc;

  Matrix projection_left, projection_right;
  Matrix pos_left, pos_right;
  int render_width, render_height;
  int m_fNearClip = 0.01;
  int m_fFarClip = 30000.0;
  int m_iValidPoseCount = 0;
  std::string m_strPoseClasses = "";
};
void VR::HandleEvent()
{
  if (hmd) {
    vr::VREvent_t event;
    while( hmd->PollNextEvent( &event, sizeof(event) ) ) {
      ProcessVREvent( event );
    }
  }
}
void VR::ProcessVREvent(vr::VREvent_t event)
{
  switch(event.eventType) {
  case vr::VREvent_TrackedDeviceDeactivated:
    {
      int i = event.trackedDeviceIndex;
      std::cout << "Device " << i << " deactivated" << std::endl;
    }
    break;
  case vr::VREvent_TrackedDeviceUpdated:
    {
      int i = event.trackedDeviceIndex;
      std::cout << "Device " << i << " updated" << std::endl;      
    }
    break;
  }
}

void VR::UpdateActionState()
{
  //vr::VRActiveActionSet_t actionSet = { 0 };
  //actionSet.ulActionSet = m_actionsetDemo;
  //vr::VRInput()->UpdateActionState( &actionSet, sizeof(actionSet), 1);
}

void VR::CreateFrameBuffers() {
  CreateFrameBuffer(800,600, leftEyeDesc);
  CreateFrameBuffer(800,600, rightEyeDesc);
}
void VR::InitVR() {
  vr::EVRInitError eError = vr::VRInitError_None;
  hmd = vr::VR_Init( &eError, vr::VRApplication_Scene );
  if (eError != vr::VRInitError_None) {
    std::cout << "ERROR: virtual reality initialization failed!" << std::endl;
  }

  if (!vr::VRCompositor() )
    {
      std::cout << "VR Compositor initialization failed!\n" << std::endl;
    }

}
void VR::Cleanup()
{
  if (hmd) { vr::VR_Shutdown(); hmd=NULL; }
}
void VR::Submit()
{
  Submit(leftEyeDesc.m_nResolveTextureId, rightEyeDesc.m_nResolveTextureId);
}
void VR::Submit(int texid_left, int texid_right)
{
  if (hmd) {
    vr::Texture_t leftTexture = { (void*)(uintptr_t)texid_left, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::VRCompositor()->Submit(vr::Eye_Left, &leftTexture);
    
    vr::Texture_t rightTexture = { (void*)(uintptr_t)texid_right, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::VRCompositor()->Submit(vr::Eye_Right, &rightTexture);
  }
  glFinish(); // fix jitter issue
}

bool VR::CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc)
{
  glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId );
  glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);
  
   glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
   glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
   glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight );
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,	framebufferDesc.m_nDepthBufferId );
   
   glGenTextures(1, &framebufferDesc.m_nRenderTextureId );
   glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId );
   glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);
   
   glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId );
   glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);
   
   glGenTextures(1, &framebufferDesc.m_nResolveTextureId );
   glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId );
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);
   
   // check FBO status
   GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (status != GL_FRAMEBUFFER_COMPLETE)
     {
       return false;
     }
   
   glBindFramebuffer( GL_FRAMEBUFFER, 0 );

   return true;
}

Matrix VR::ConvertMatrix( const vr::HmdMatrix34_t &pose)
{
  Matrix m = { pose.m[0][0], pose.m[1][0], pose.m[2][0], 0.0f,
	       pose.m[0][1], pose.m[1][1], pose.m[2][1], 0.0f,
	       pose.m[0][2], pose.m[1][2], pose.m[2][2], 0.0f,
	       pose.m[0][3], pose.m[1][3], pose.m[2][3], 1.0f, false };
  return m;
}

void VR::SetupCameras()
{
  projection_left = GetHMDMatrixProjectionEye( vr::Eye_Left );
  projection_right = GetHMDMatrixProjectionEye( vr::Eye_Right );
  pos_left = GetHMDMatrixPoseEye( vr::Eye_Left );
  pos_right = GetHMDMatrixPoseEye( vr::Eye_Right );
}
Matrix VR::GetCurrentViewProjectionMatrix( vr::Hmd_Eye nEye )
{
  if (nEye == vr::Eye_Left) {
    return projection_left * pos_left * hmd_pose;
  }
  if (nEye == vr::Eye_Right) {
    return projection_right * pos_right * hmd_pose;
  }
  return Matrix::Identity();
}
Matrix VR::GetHMDMatrixProjectionEye( vr::Hmd_Eye nEye )
{
  if (!hmd) { return Matrix::Identity(); }
  vr::HmdMatrix44_t mat = hmd->GetProjectionMatrix( nEye, m_fNearClip, m_fFarClip );
  Matrix m = { mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
	       mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
	       mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
	       mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3], false };
  return m;
}
Matrix VR::GetHMDMatrixPoseEye( vr::Hmd_Eye nEye )
{
  if (!hmd) { return Matrix::Identity(); }
  vr::HmdMatrix34_t mat = hmd->GetEyeToHeadTransform( nEye );
  Matrix m = { mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0,
	       mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0,
	       mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0,
	       mat.m[0][3], mat.m[1][3], mat.m[2][3], 0.0, false };
  return m;
}

void VR::UpdatePose()
{
  if (!hmd) return;
  
  vr::VRCompositor()->WaitGetPoses(pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
  
  m_iValidPoseCount = 0;
  m_strPoseClasses = "";
  int s = vr::k_unMaxTrackedDeviceCount;
  for(int nDevice = 0; nDevice < s; ++nDevice) 
    {
      if (pose[nDevice].bPoseIsValid ) {
	m_iValidPoseCount++;
	device_pose[nDevice] = ConvertMatrix( pose[nDevice].mDeviceToAbsoluteTracking );
	
      }
    }
  if (pose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid ) {
    hmd_pose = device_pose[vr::k_unTrackedDeviceIndex_Hmd];
    hmd_pose = Matrix::Inverse(hmd_pose);
  }
}

void VR::RenderStereoTargets(std::function<void()> render_left, std::function<void()> render_right)
{
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_MULTISAMPLE );
  
  // left eye
  glBindFramebuffer( GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId );
  glViewport(0,0,render_width, render_height);
  render_left();
  glBindFramebuffer(GL_FRAMEBUFFER, 0 );
  glDisable( GL_MULTISAMPLE );
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId);
  glBlitFramebuffer(0,0,render_width, render_height, 0,0,render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

  glEnable(GL_MULTISAMPLE);

  // right eye
  glBindFramebuffer( GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
  glViewport(0,0,render_width, render_height);
  render_right();
  glBindFramebuffer(GL_FRAMEBUFFER,0);

  glDisable(GL_MULTISAMPLE);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId );
  
  glBlitFramebuffer( 0,0, render_width, render_height, 0,0, render_width, render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR );
  glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
  glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
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

extern int async_pending_count_previous;
extern int async_pending_count;
extern std::string gameapi_seamless_url;
extern int score;
extern int hidden_score;

class MainLoopSplitter_vr : public Splitter
{
public:
  MainLoopSplitter_vr(GameApi::ML code, bool logo, bool fpscounter, float start_time, float duration, int screen_width, int screen_height) : code(code), logo(logo), fpscounter(fpscounter), timeout(duration), start_time(start_time), screen_width(screen_width), screen_height(screen_height)
  {
  }
  virtual void set_env(GameApi::Env *ei)
  {
    e = ei;
  }
  virtual void set_everyapi(GameApi::EveryApi *evi)
  {
    ev = evi;
  }
  virtual void Init()
  {
    score = 0;
    hidden_score = 0;


    vr.InitVR();
    vr.CreateFrameBuffers();

    Envi_2 &env = envi;
    env.logo_shown = logo;
    env.fpscounter = fpscounter;
    env.timeout = start_time+timeout;
    env.start_time = start_time;
    env.screen_width = screen_width;
    env.screen_height = screen_height;
    env.ev = ev;
    
    GameApi::SH sh = env.ev->shader_api.colour_shader();
    GameApi::SH sh2 = env.ev->shader_api.texture_shader();
    GameApi::SH sh3 = env.ev->shader_api.texture_array_shader();
    
    // rest of the initializations
    env.ev->mainloop_api.init_3d(sh);
    env.ev->mainloop_api.init_3d(sh2);
    env.ev->mainloop_api.init_3d(sh3);
    env.ev->shader_api.use(sh);
    
    GameApi::ML ml = mainloop(*env.ev);
    if (async_pending_count > 0) { env.logo_shown = true; }
    
    env.mainloop = ml;
    
    env.color_sh = sh;
    env.texture_sh = sh2;
    env.arr_texture_sh = sh3;
    
    env.ev->mainloop_api.reset_time();
    if (env.logo_shown) {
      if (gameapi_seamless_url == "") {
	env.ev->mainloop_api.display_logo(*env.ev);
      } else {
	env.ev->mainloop_api.display_seamless(*env.ev);
      }
    } else {
	env.ev->mainloop_api.advance_time(env.start_time/10.0*1000.0);
    }
     env.ev->mainloop_api.alpha(true);
     glEnable(GL_DEPTH_TEST);
     GameApi::MainLoopApi::Event e;
     while((e = env.ev->mainloop_api.get_event()).last==true)
       {
	 /* this eats all events from queue */
       }
  }
  virtual int Iter()
  {
    Envi_2 *env = (Envi_2*)&envi;
    //std::cout << "async: " << async_pending_count << std::endl;
    if (async_pending_count > 0) { env->logo_shown = true; }
    if (async_pending_count != async_pending_count_previous)
      {
	std::cout << "ASync pending count=" << async_pending_count << std::endl;
	async_pending_count_previous = async_pending_count;
      }
    if (env->logo_shown)
      {
	bool b = false;
	if (gameapi_seamless_url=="") {
	  b = env->ev->mainloop_api.logo_iter();
	} else {
	  b = env->ev->mainloop_api.seamless_iter();
	}
	if (b && async_pending_count==0) { env->logo_shown = false;
	  env->ev->mainloop_api.reset_time();
	  env->ev->mainloop_api.advance_time(env->start_time/10.0*1000.0);
	}
	return -1;
      }

    env->ev->mainloop_api.clear_3d(0xff000000);
    
    // handle esc event
    GameApi::MainLoopApi::Event e2;
    while((e2 = env->ev->mainloop_api.get_event()).last==true)
      {
	//std::cout << e.ch << " " << e.type << std::endl;
#ifndef EMSCRIPTEN
	if (e2.ch==27 && e2.type==0x300) { env->exit = true; return 0; }
#endif
	
	GameApi::InteractionApi::quake_movement_event(*env->ev,e2, env->pos_x, env->pos_y, env->rot_y,
						      env->data, env->speed_x, env->speed_y,
				   1.0, 1.0*3.14159*2.0/360.0);
	env->ev->mainloop_api.event_ml(env->mainloop, e2);
	
      }
    vr.HandleEvent();
    GameApi::InteractionApi::quake_movement_frame(*env->ev, env->pos_x, env->pos_y, env->rot_y,
						  env->data, env->speed_x, env->speed_y,
						  1.0, 1.0*3.14159*2.0/360.0);
    
    GameApi::M mat = env->ev->matrix_api.identity();
    env->ev->shader_api.use(env->color_sh);
    env->ev->shader_api.set_var(env->color_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->color_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    env->ev->shader_api.use(env->texture_sh);
    env->ev->shader_api.set_var(env->texture_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    
    env->ev->shader_api.use(env->arr_texture_sh);
    env->ev->shader_api.set_var(env->arr_texture_sh, "in_MV", mat);
    //env->ev->shader_api.set_var(env->arr_texture_sh, "in_iMV", env->ev->matrix_api.transpose(env->ev->matrix_api.inverse(mat)));
    env->ev->shader_api.use(env->color_sh);
    
    GameApi::M in_MV = mat; //env->ev->mainloop_api.in_MV(*env->ev, true);
    GameApi::M in_T = env->ev->mainloop_api.in_T(*env->ev, true);
    GameApi::M in_N = env->ev->mainloop_api.in_N(*env->ev, true);
    
    //env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N, env->screen_width, env->screen_height);

    GameApi::Env *ee = e;
    if (vr.Initialized()) {
    vr.RenderStereoTargets([ee,env,this,in_T,in_N](){
	Matrix m = vr.GetCurrentViewProjectionMatrix(vr::Eye_Left);
	GameApi::M m2 = add_matrix2(*ee, m);
	env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, m2, in_T, in_N, env->screen_width, env->screen_height);
      },
      [ee,env,this,in_T,in_N]() {
	Matrix m = vr.GetCurrentViewProjectionMatrix(vr::Eye_Right);
	GameApi::M m2 = add_matrix2(*ee, m);

	env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, m2, in_T, in_N, env->screen_width, env->screen_height);
      });
    vr.Submit();
    vr.UpdatePose();
    vr.SetupCameras();
    } else {
      env->ev->mainloop_api.execute_ml(env->mainloop, env->color_sh, env->texture_sh, env->texture_sh, env->arr_texture_sh, in_MV, in_T, in_N, env->screen_width, env->screen_height);

    }

    if (env->fpscounter)
      env->ev->mainloop_api.fpscounter();
    if (env->ev->mainloop_api.get_time()/1000.0*10.0 > env->timeout)
      {
	env->exit = true;
      }
    
    // swapbuffers
    env->ev->mainloop_api.swapbuffers();
    return -1;
  }
  virtual void Destroy()
  {
    // this is needed for win32 build in editor
      glDisable(GL_DEPTH_TEST);
  }
  virtual Splitter* NextState(int code)
  {
    return 0;
  }
  GameApi::ML mainloop(GameApi::EveryApi &ev)
  {
    return code;
  }

private:
  GameApi::ML code;
  bool logo;
  bool fpscounter;
  float timeout;
  float start_time;
  int screen_width;
  int screen_height;
  Envi_2 envi;
  VR vr;
};



EXPORT GameApi::RUN GameApi::BlockerApi::vr_window(GameApi::EveryApi &ev, ML ml, bool logo, bool fpscounter, float start_time, float duration)
{
  Splitter *spl = new MainLoopSplitter_vr(ml,logo, fpscounter, start_time, duration, ev.mainloop_api.get_screen_sx(), ev.mainloop_api.get_screen_sy());
  return add_splitter(e, spl);
}

#endif
