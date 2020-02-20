
#include "GameApi_h.hh"
#include "GameApi_low.hh"
#include "Buffer.hh"


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

#else // emscripten

#endif

#ifndef EMSCRIPTEN
vr::IVRSystem *hmd = 0;
#endif

EXPORT void    check_vr_overlay_init()
{
#ifndef EMSCRIPTEN
  vr::EVRInitError aError = vr::VRInitError_None;
  hmd = vr::VR_Init( &aError, vr::VRApplication_Overlay );
  std::cout << "VR_Init errorcode: " << aError << std::endl;
  std::cout << "hmd = " << hmd << std::endl;
#endif
}


#if 0
class OpenVRWebCamHeavy : public HeavyOperation
{
public:
  OpenVRWebCamHeavy(GameApi::EveryApi &ev, int num) : ev(ev),camera_num(num) { }
public:
  virtual void* get_data(std::string type)
  {
    if (type=="BufferRef") { return &ref; }
    if (type=="TXID") {
      id = ev.texture_api.bufferref_to_txid(id,ref);
      return &id;
    }
  }
private:
  GameApi::EveryApi &ev;
  int camera_num;
  BufferRef ref;
  GameApi::TXID id;
};
#endif

class WebCamBufferRef : public BufferRefReq
{
public:
  WebCamBufferRef(int num) : num(num), done(false) { }
  BufferRef Buffer() const { 
    vr::EVRTrackedCameraFrameType frametype = vr::VRTrackedCameraFrameType_Distorted;
    if (!done && vr::VRTrackedCamera()) {
      vr::TrackedDeviceIndex_t idx = num;
      vr::VRTrackedCamera()->AcquireVideoStreamingService( idx, &camerahandle );
      unsigned int width=1, height=1;
      vr::VRTrackedCamera()->GetCameraFrameSize( idx, frametype, &width, &height, &framebuffersize );
      buf = BufferRef::NewBuffer(width,height);
      done = true;
    }
    vr::CameraVideoStreamFrameHeader_t frameheader;
    if (vr::VRTrackedCamera()) 
      vr::VRTrackedCamera()->GetVideoStreamFrameBuffer( camerahandle, frametype, buf.buffer, framebuffersize, &frameheader, sizeof(frameheader) );
    
    // TODO, DO WE NEED TO SWAP THE BYTES IN ANY WAY?
  
    return buf;
  }
  ~WebCamBufferRef() { BufferRef::FreeBuffer(buf); }
private:
  int num;
  mutable bool done;
  mutable BufferRef buf;
  mutable vr::TrackedCameraHandle_t camerahandle;
  mutable unsigned int framebuffersize=0;
};

class BufferRefOp : public BufferRefReq
{
public:
  BufferRefOp(BufferRefReq &seq) : seq(seq) { }
  BufferRef Buffer() const {
    BufferRef ref = seq.Buffer();
    // do some oeprations here
    int sx = ref.width;
    int sy = ref.height;
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	{
	  unsigned int color = ref.buffer[x+y*ref.ydelta];
	  if ((color&0xff000000) < 0x80000000) color = 0x0;
	  if ((color&0xff0000) < 0x800000) color = 0x0;
	  if ((color&0xff00) < 0x8000) color = 0x0;
	  ref.buffer[x+y*ref.ydelta] = color; 
	  }
    // ...

    return ref;
  }
private:
  BufferRefReq &seq;
};

class WebCamBufferRefTexID : public TextureID
{
public:
  WebCamBufferRefTexID(GameApi::EveryApi &ev, BufferRefReq &seq) : ev(ev), seq(seq) { id.id=0; }
  void handle_event(MainLoopEvent &e) {
  }
  void render(MainLoopEnv &e) {
    id = ev.texture_api.bufferref_to_txid(id, seq.Buffer());
  }
  int texture() const
  {
    return id.id;
  }
private:
  GameApi::EveryApi &ev;
  BufferRefReq &seq;
  mutable GameApi::TXID id;
};

class WebCamTexID : public TextureID
{
public:
  WebCamTexID(int num) : num(num) {
    done = false;
  }
  void handle_event(MainLoopEvent &e)
  {
  }
  void render(MainLoopEnv &e) {
    vr::EVRTrackedCameraFrameType frametype = vr::VRTrackedCameraFrameType_Distorted;
    vr::CameraVideoStreamFrameHeader_t frameheader;
    vr::TrackedDeviceIndex_t idx = num;
    if (!done && vr::VRTrackedCamera()) {
      vr::VRTrackedCamera()->AcquireVideoStreamingService( idx, &camerahandle );
      done = true;
  }
    if (vr::VRTrackedCamera())
      vr::VRTrackedCamera()->GetVideoStreamTextureGL( camerahandle, frametype, &gl_id, &frameheader, sizeof(frameheader) );
    if (gl_id!=-1) {
      id.id = gl_id;
      g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, id.id);
      g_low->ogl->glDisable(Low_GL_DEPTH_TEST);
    }
  }
  int texture() const
  {
    return id.id;
  }
private:
  int num;
  unsigned int gl_id = -1;
  vr::TrackedCameraHandle_t camerahandle;
  bool done;
  mutable GameApi::TXID id;
};

GameApi::TXID GameApi::TextureApi::webcam_txid(int num)
{
  return add_txid(e, new WebCamTexID(num));
}

class VR_Overlay : public MainLoopItem
{
public:
  VR_Overlay(GameApi::Env &env, GameApi::TXID id, std::string key, std::string name) : env(env), id(id), key(key), name(name) { done=false; done2=false;}
  virtual void Prepare()
  {
    if (!done && vr::VROverlay()) {
      vr::VROverlay()->CreateOverlay( key.c_str(), name.c_str(), &overlay);
      done = true;
    }
  }
  virtual void execute(MainLoopEnv &e)
  {

    TextureID *iid = find_txid(env, id);
    iid->render(e);

    int tex = iid->texture();
    if (old_id!=tex && tex!=-1 && tex!=0) {
      vr::Texture_t texture = { (void*)tex, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
      if (vr::VROverlay())
	vr::VROverlay()->SetOverlayTexture(overlay, &texture);
      old_id = tex;
    }
    if (!done2) {
      if (vr::VROverlay())
	vr::VROverlay()->ShowOverlay(overlay);
      done2 = true;
    }

  }
  virtual void handle_event(MainLoopEvent &e)
  {
    TextureID *iid = find_txid(env, id);
    iid->handle_event(e);
  }
  virtual int shader_id() { return -1; }

private:
  GameApi::Env &env;
  bool done, done2;
  GameApi::TXID id;
  int old_id=-1;
  vr::VROverlayHandle_t overlay;
  std::string key, name;
};

GameApi::ML GameApi::TextureApi::vr_overlay(GameApi::TXID id, std::string key, std::string name)
{
  return add_main_loop(e, new VR_Overlay(e,id, key, name));
}

// TODO: ML->RUN that uses openvr's PollEvent().
