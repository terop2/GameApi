
#include "GameApi_h.hh"
#include "GameApi_low.hh"
#include "Buffer.hh"



class WebCamBufferRefTexID : public TextureID
{
public:
  WebCamBufferRefTexID(GameApi::EveryApi &ev, BufferRefReq &seq) : ev(ev), seq(seq) { id.id=0; }
  void handle_event(MainLoopEvent &e) {
  }
  void render(MainLoopEnv &e) {
  }
  bool is_fbo() const { return false; }
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


#ifndef LINUX

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
  virtual std::vector<int> shader_id() { return std::vector<int>(); }

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


#endif // !LINUX


#endif

#ifndef EMSCRIPTEN
#ifndef LINUX

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
#endif
#endif

#ifdef LINUX

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

class LinuxCapture : public BufferRefReq
{
public:
  LinuxCapture(int sx, int sy, int num) : num(num) {
    sx=640; sy=480;
    ref = BufferRef::NewBuffer(sx,sy);
    init_done = false;

   
  }
  ~LinuxCapture()
  {
    if (init_done) {
      munmap(buffer,buf.length);
      close(fd);
    }
  }
  void initCapture(int num) const
  {
    const char* dev_name = "/dev/video0";
    fd = open(dev_name, O_RDWR);
    if (fd == -1) {
        perror("Opening video device");
        return;
    }

    v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
        perror("Querying Capabilities");
        return;
    }

    std::cout << "Driver Caps:\n"
              << "  Driver: " << cap.driver << "\n"
              << "  Card: " << cap.card << "\n"
              << "  Bus: " << cap.bus_info << "\n"
              << "  Version: " << ((cap.version >> 16) & 0xFF) << "."
              << ((cap.version >> 8) & 0xFF) << "."
              << (cap.version & 0xFF) << "\n";

    // Set Image Format
    v4l2_format fmt{};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Setting Pixel Format");
        return;
    }

    // Request Buffer
    v4l2_requestbuffers req{};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Requesting Buffer");
        return;
    }

    // Query Buffer
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
        perror("Querying Buffer");
        return;
    }

    buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        return;
    }


  }

inline uint8_t clamp(int value) const {
    return std::max(0, std::min(255, value));
}

// Converts a YUYV buffer (width * height * 2 bytes) to ARGB (width * height * 4 bytes)
  void convertYUYVtoARGB(uint8_t* yuyv, uint8_t* argb, int width, int height) const {
    int numPixels = width * height;

    for (int i = 0, j = 0; i < numPixels * 2; i += 4, j += 8) {
        // Extract YUYV for 2 pixels
        uint8_t y0 = yuyv[i + 0];
        uint8_t u  = yuyv[i + 1];
        uint8_t y1 = yuyv[i + 2];
        uint8_t v  = yuyv[i + 3];

        int c0 = y0 - 16;
        int c1 = y1 - 16;
        int d = u - 128;
        int e = v - 128;

        // First pixel
        int r = clamp((298 * c0 + 409 * e + 128) >> 8);
        int g = clamp((298 * c0 - 100 * d - 208 * e + 128) >> 8);
        int b = clamp((298 * c0 + 516 * d + 128) >> 8);

        argb[j + 0] = r;  // A
        argb[j + 1] = g;
        argb[j + 2] = b;
        argb[j + 3] = 255;

        // Second pixel
        r = clamp((298 * c1 + 409 * e + 128) >> 8);
        g = clamp((298 * c1 - 100 * d - 208 * e + 128) >> 8);
        b = clamp((298 * c1 + 516 * d + 128) >> 8);

        argb[j + 4] = r;  // A
        argb[j + 5] = g;
        argb[j + 6] = b;
        argb[j + 7] = 255;
    }
}
  
  void doCapture(int num) const
  {

    // Queue the Buffer
    if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
        perror("Queue Buffer");
        return;
    }

    // Start Streaming
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        perror("Start Capture");
        return;
    }
    if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
        perror("Retrieving Frame");
        return;
    }
    //BufferRef::FreeBuffer(ref);
    unsigned char *buffer2 = (unsigned char *)buffer;
    std::vector<unsigned char> s = std::vector<unsigned char>(buffer2,buffer2+buf.bytesused);
    //std::cout << "BYTES USED:" << buf.bytesused << std::endl;
    //std::cout << std::string(s.begin(),s.begin()+30) << std::endl;
    //bool success=false;
    //ref = LoadImageFromString(s,success);    
    int sx = ref.width;
    int sy = ref.height;
    convertYUYVtoARGB(&s[0],(uint8_t*)ref.buffer,sx,sy);
    for(int y=0;y<sy/2;y++)
      for(int x=0;x<sx;x++)
	{
	  std::swap(*(ref.buffer+x+y*ref.ydelta),*(ref.buffer+x+(ref.height-y-1)*ref.ydelta));
	}
  }
  
  
  void Gen() const { }
  BufferRef Buffer() const {
    if (!init_done) {
      initCapture(num);
      init_done = true;
    }
    doCapture(num);
    return ref;
  }
private:
  mutable BufferRef ref;
  mutable int num;
  mutable bool init_done;
  mutable int fd;
  mutable v4l2_buffer buf;
  mutable void *buffer=0;
  
};


GameApi::TXID GameApi::TextureApi::webcam_txid_linux(EveryApi &ev, int sx, int sy, int num)
{
  LinuxCapture *buf = new LinuxCapture(sx,sy,num);
  WebCamBufferRefTexID *id = new WebCamBufferRefTexID(ev, *buf);
  return add_txid(e,id);
}

#endif

GameApi::TXID GameApi::TextureApi::webcam_txid_generic(EveryApi &ev, int sx, int sy, int num)
{
#ifdef LINUX
  return webcam_txid_linux(ev,sx,sy,num);
#endif
#ifdef WINDOWS
  return webcam_txid_win(ev,sx,sy,num);
#endif
}
