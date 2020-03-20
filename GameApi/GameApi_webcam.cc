
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


class WebCamBufferRefTexID : public TextureID
{
public:
  WebCamBufferRefTexID(GameApi::EveryApi &ev, BufferRefReq &seq) : ev(ev), seq(seq) { id.id=0; }
  void handle_event(MainLoopEvent &e) {
  }
  void render(MainLoopEnv &e) {
  }
  int texture() const
  {
    id = ev.texture_api.bufferref_to_txid(id, seq.Buffer());
    return id.id;
  }
private:
  GameApi::EveryApi &ev;
  BufferRefReq &seq;
  mutable GameApi::TXID id;
};


#ifndef VIRTUAL_REALITY_OVERLAY
EXPORT void    check_vr_overlay_init()
{
}
#else

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
  WebCamBufferRef(int num) : num(num), done(false) { 
    frameheader.nFrameSequence = 0;
  }
  void Gen() const { }
  BufferRef Buffer() const { 
    vr::EVRTrackedCameraFrameType frametype = vr::VRTrackedCameraFrameType_Undistorted;
    if (!done && vr::VRTrackedCamera()) {
      vr::TrackedDeviceIndex_t idx = num;
      vr::VRTrackedCamera()->AcquireVideoStreamingService( idx, &camerahandle );
      unsigned int width=1, height=1;
      vr::VRTrackedCamera()->GetCameraFrameSize( idx, frametype, &width, &height, &framebuffersize );
      buf = BufferRef::NewBuffer(width,height);
      done = true;


    }
    if (vr::VRTrackedCamera()) 
      vr::VRTrackedCamera()->GetVideoStreamFrameBuffer( camerahandle, frametype, 0, 0, &frameheader, sizeof(frameheader) );
    if (frameheader.nFrameSequence == seq) return buf;
    vr::EVRTrackedCameraError err;
    if (vr::VRTrackedCamera()) 
      err = vr::VRTrackedCamera()->GetVideoStreamFrameBuffer( camerahandle, frametype, buf.buffer, framebuffersize, &frameheader, sizeof(frameheader) );
    //std::cout << "Err: " << err << std::endl;
    seq = frameheader.nFrameSequence;
    //std::cout << "GetBuf" << std::endl;
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
  mutable vr::CameraVideoStreamFrameHeader_t frameheader;
  mutable int seq=0;
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




GameApi::TXID GameApi::TextureApi::webcam_txid_slow(EveryApi &ev, int num)
{
  WebCamBufferRef *buf = new WebCamBufferRef(num);
  WebCamBufferRefTexID *id = new WebCamBufferRefTexID(ev, *buf);
  return add_txid(e, id);
}


class WebCamTexID : public TextureID
{
public:
  WebCamTexID(int num) : num(num) {
    done = false;
    id.id=0;
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
      {
      vr::EVRTrackedCameraError err;
      err = vr::VRTrackedCamera()->GetVideoStreamTextureGL( camerahandle, frametype, &gl_id, &frameheader, sizeof(frameheader) );
      std::cout << "VideoStreamTexture:" << err << std::endl;
      }
    if (gl_id!=0) {
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
  unsigned int gl_id = 0;
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
  VR_Overlay(GameApi::Env &env, GameApi::TXID id, std::string key, std::string name, int sx, int sy) : env(env), id(id), key(key), name(name),sx(sx),sy(sy) { done=false; done2=false;}
  ~VR_Overlay() {
    if (vr::VROverlay())
      vr::VROverlay()->DestroyOverlay( overlay );
    if (vr::VROverlay())
      vr::VROverlay()->DestroyOverlay( overlay2 );
  }
  virtual void Prepare()
  {
    if (!done && vr::VROverlay()) {
      vr::VROverlayError err;
      err = vr::VROverlay()->CreateDashboardOverlay( key.c_str(), name.c_str(), &overlay, &overlay2);
      std::cout << "Overlay error: " << err << std::endl;

      vr::VROverlay()->SetOverlayWidthInMeters( overlay, 1.5f );
      vr::VRTextureBounds_t bounds = { 0.0, 0.0, 1.0, 1.0 };
      vr::VROverlay()->SetOverlayTextureBounds( overlay, &bounds );
      vr::VROverlay()->SetOverlayInputMethod( overlay, vr::VROverlayInputMethod_Mouse );
      vr::HmdVector2_t win = {
	(float)sx,
	(float)sy
      };
      vr::VROverlay()->SetOverlayMouseScale( overlay, &win );
      done = true;
    }
  }
  virtual void execute(MainLoopEnv &e)
  {

    TextureID *iid = find_txid(env, id);
    iid->render(e);

    int tex = iid->texture();
    //if (old_id!=tex && tex!=-1 && tex!=0) {
    {
      g_low->ogl->glBindTexture(Low_GL_TEXTURE_2D, tex);

      vr::Texture_t texture = { (void*)(uintptr_t)tex, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
      if (vr::VROverlay())
	vr::VROverlay()->SetOverlayTexture(overlay, &texture);
      //if (vr::VROverlay())
      //	vr::VROverlay()->SetOverlayTexture(overlay, &texture);
      old_id = tex;
    }
    if (!done2) {
      //if (vr::VROverlay())
      //	vr::VROverlay()->ShowOverlay(overlay);
      done2 = true;
    }
    vr::VREvent_t event;
    while ( vr::VROverlay()->PollNextOverlayEvent( overlay, &event, sizeof(event) ) ) { }
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
  vr::VROverlayHandle_t overlay2;
  std::string key, name;
  int sx,sy;
};

GameApi::ML GameApi::TextureApi::vr_overlay(GameApi::TXID id, std::string key, std::string name, int sx, int sy)
{
  return add_main_loop(e, new VR_Overlay(e,id, key, name,sx,sy));
}

// TODO: ML->RUN that uses openvr's PollEvent().



#endif

#include "escapi/escapi.h"

class ESCCapture : public BufferRefReq
{
public:
  ESCCapture(int sx, int sy, int num) : num(num) {
    ref = BufferRef::NewBuffer(sx,sy);
    params.mTargetBuf = (int*)ref.buffer;
    params.mWidth = sx;
    params.mHeight = sy;

    count = setupESCAPI();
    init_done = false;
  }
  ~ESCCapture() { if (init_done) deinitCapture(num); BufferRef::FreeBuffer(ref); }
  void Gen() const { }
  BufferRef Buffer() const { 
    if (num>=0 && num<count) {
      if (!init_done) {
	initCapture(num, &params);
	init_done = true;
      }
      doCapture(num);
      while(isCaptureDone(num)!=1) { }
      return ref;
    } else {
      return ref;
    }
  }
private:
  mutable BufferRef ref;
  mutable SimpleCapParams params;
  int count;
  int num;
  mutable bool init_done;
};

class BufferRefFlip : public BufferRefReq
{
public:
  BufferRefFlip(BufferRefReq &seq) : seq(seq) { }
  void Gen() const { }
  BufferRef Buffer() const {
    BufferRef ref = seq.Buffer();
    // do some oeprations here
    int sx = ref.width;
    int sy = ref.height;
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx;x++)
	{
	  unsigned int color = ref.buffer[x+y*ref.ydelta];
	  unsigned char *ptr = (unsigned char*)&color;
	  unsigned char b = ptr[0];
	  unsigned char g = ptr[1];
	  unsigned char r = ptr[2];
	  unsigned char a = ptr[3];
	  ptr[0]=r;
	  ptr[1]=g;
	  ptr[2]=b;
	  ptr[3]=a;
	  ref.buffer[x+y*ref.ydelta] = color; 
	  }
    // ...

    return ref;
  }
private:
  BufferRefReq &seq;
};


GameApi::TXID GameApi::TextureApi::webcam_txid_win(EveryApi &ev, int sx, int sy, int num)
{
  ESCCapture *buf = new ESCCapture(sx,sy,num);
  BufferRefFlip *buf2 = new BufferRefFlip(*buf);
  WebCamBufferRefTexID *id = new WebCamBufferRefTexID(ev, *buf2);
  return add_txid(e,id);
}
