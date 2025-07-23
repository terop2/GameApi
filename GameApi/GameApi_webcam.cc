
#include "GameApi_h.hh"
#include "GameApi_low.hh"
#include "Buffer.hh"



class WebCamBufferRefTexID : public TextureID
{
public: // THIS IS STRANGE CLASS AS IT DELETES ITS seq PARAMETER.
  WebCamBufferRefTexID(GameApi::EveryApi &ev, BufferRefReq &seq) : ev(ev), seq(seq) { id.id=0; }
  ~WebCamBufferRefTexID() { delete &seq; } 
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
      std::cout << "Stopping webcam...(try)" << std::endl;
    if (init_done) {
      std::cout << "Stopping webcam..." << std::endl;
      int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      ioctl(fd, VIDIOC_STREAMOFF, &type);


    struct v4l2_requestbuffers req = {0};
    req.count = 0;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ioctl(fd, VIDIOC_REQBUFS, &req);
      
      munmap(buffer,buf.length);
      close(fd);
    }
  }
  void initCapture(int num) const
  {
    std::cout << "Starting webcam.." << std::endl;
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
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx/2;x++)
	{
	  std::swap(*(ref.buffer+x+y*ref.ydelta),*(ref.buffer+(ref.width-x-1)+y*ref.ydelta));
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

#ifdef WINDOWS
#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <wrl/client.h>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#define INITGUID
#include <initguid.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <objbase.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "shlwapi.lib")

#if 0
DEFINE_GUID(MF_MT_MAJOR_TYPE,       0x48eba18e,0xf8c9,0x4687,0xbf,0x11,0x0a,0x74,0xc9,0xf9,0x6a,0x8f);
DEFINE_GUID(MF_MT_SUBTYPE,          0xf7e34c9a,0x42e8,0x4714,0xb7,0xc6,0x5a,0x1c,0x64,0x66,0x21,0x8f);
DEFINE_GUID(MFMediaType_Video,      0x73646976,0x0000,0x0010,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71);
DEFINE_GUID(MFVideoFormat_RGB32,    0x00000016,0x0000,0x0010,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71);
#endif

using namespace Microsoft::WRL;

class VideoPlayer_win32 : public BufferRefReq
{
public:
  VideoPlayer_win32(int sx, int sy, std::string url) : url(url), sx(sx), sy(sy) {
    ref=BufferRef::NewBuffer(sx,sy);

  }
  ~VideoPlayer_win32() {
    if (init_done) {
      MFShutdown();
      //CoUninitialize();
      BufferRef::FreeBuffer(ref);
    }
  }
  void print_guid(const GUID& guid) const {
    wchar_t guidString[39] = {0}; // 38 chars + null
    if (StringFromGUID2(guid, guidString, 39)) {
        std::wcout << L"GUID: " << guidString << std::endl;
    } else {
        std::wcout << L"GUID: <conversion failed>" << std::endl;
    }
}
  void check(HRESULT hr, std::string pos) const
  {
    if (FAILED(hr)) std::cout << "Failed: " << pos << std::endl;
  }
  void init() const {
    HRESULT hr;
    //hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    //check(hr,"CoInitialize");
   hr=MFStartup(MF_VERSION);
   check(hr,"MFStartup");

    ComPtr<IMFAttributes> attr;
    hr=MFCreateAttributes(&attr, 1);
   check(hr,"MFCreateAttributes");
    attr->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
    std::wstring ws(url.begin(),url.end());
    hr=MFCreateSourceReaderFromURL(ws.c_str(), attr.Get(), &reader);
   check(hr,"MFCreateSourceReaderFromURL");

    ComPtr<IMFMediaType> type;
    hr=MFCreateMediaType(&type);
   check(hr,"CreateMediaType");
    hr=type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
   check(hr,"SetGUID Video");

    hr=type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
   check(hr,"SetGUID RGB32");

   //hr=MFSetAttributeSize(type.Get(),MF_MT_FRAME_SIZE,sx,sy);
   //check(hr,"framesize");
   //hr=MFSetAttributeRatio(type.Get(),MF_MT_FRAME_RATE,30,1);
   //check(hr,"framerate");
   
    hr=reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, type.Get());
    check(hr,"SetCurrentMediaType");


    
    ComPtr<IMFMediaType> actualType;
    reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &actualType);

    GUID subtype = {};
    actualType->GetGUID(MF_MT_SUBTYPE, &subtype);
    std::wcout << L"Actual subtype: ";
    print_guid(subtype);
    std::wcout << std::endl;
    
    UINT32 targetWidth = 640;
    UINT32 targetHeight = 480;
    UINT64 size=0;
    hr=MFGetAttributeSize(actualType.Get(), MF_MT_FRAME_SIZE, &targetWidth, &targetHeight);
    check(hr,"GetFrameSize");
    
    sx=targetWidth;
    sy=targetHeight;
    BufferRef::FreeBuffer(ref);
    ref=BufferRef::NewBuffer(sx,sy);


    UINT32 rotation = 0;
    if (SUCCEEDED(actualType->GetUINT32(MF_MT_VIDEO_ROTATION, &rotation))) {
      //std::cout << "Video rotation: " << rotation << " degrees" << std::endl;
      rot = rotation; 
    } else {
      //std::cout << "No rotation metadata found (default 0°)" << std::endl;
    }


    if(rot==270)
      {
	rotref1=BufferRef::NewBuffer(sy,sx);
      }
    if(rot==180)
      {
	rotref1=BufferRef::NewBuffer(sy,sx);
	rotref2=BufferRef::NewBuffer(sx,sy);
      }
    if(rot==90)
      {
	rotref1=BufferRef::NewBuffer(sy,sx);
	rotref2=BufferRef::NewBuffer(sx,sy);
	rotref3=BufferRef::NewBuffer(sy,sx);
      }
    
    
    std::cout << "Media size: " << sx << " " << sy << std::endl;
    

    init_done = true;
  }
  bool update_frame() const {
    if (!init_done) return false;

    IMFSample *sample = nullptr;
    DWORD streamIndex, flags;
    LONGLONG timestamp;

    HRESULT hr = reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &streamIndex, &flags, &timestamp, &sample);
    check(hr,"ReadSample");
    if (hr==MF_E_END_OF_STREAM) {
      HRESULT hr = S_OK;
      PROPVARIANT var = {};
      PropVariantInit(&var);
      var.vt = VT_I8;
      var.hVal.QuadPart = 0;
      hr = pSourceReader->SetCurrentPosition(GUID_NULL, var);
      check(hr,"setcurrentposition");
    }

    if (!sample) return false;

    IMFMediaBuffer* mediaBuffer = nullptr;
    hr=sample->ConvertToContiguousBuffer(&mediaBuffer);
   check(hr,"ConvertToContiguousBuffer");

    BYTE *data = nullptr;
    DWORD maxLength = 0, currentLength = 0;
    hr=mediaBuffer->Lock(&data, &maxLength, &currentLength);
   check(hr,"Lock");
   //std::cout << "Frame size: " << currentLength << std::endl;
   int stride = sx*4;
    for (int y=0;y<sy; y++)
      {
	  memcpy(&ref.buffer[y*ref.ydelta], &data[y*stride],stride);
      }
   
    if(rot==270)
      {
	for (int y = 0; y < sy; y++) {
	  for (int x = 0; x < sx; x++) {
	    int src_index = y * sx + x;
	    int dst_index = x * sy + (sy - 1 - y); // rotate 90° clockwise
	    
	    ((uint32_t*)rotref1.buffer)[dst_index] = ((uint32_t*)ref.buffer)[src_index];
	  }
	}
      }
    if(rot==180)
      {
	for (int y = 0; y < sy; y++) {
	  for (int x = 0; x < sx; x++) {
	    int src_index = y * sx + x;
	    int dst_index = x * sy + (sy - 1 - y); // rotate 90° clockwise
	    
	    ((uint32_t*)rotref1.buffer)[dst_index] = ((uint32_t*)ref.buffer)[src_index];
	  }
	}


	for (int y = 0; y < sx; y++) {
	  for (int x = 0; x < sy; x++) {
	    int src_index = y * sy + x;
	    int dst_index = x * sx + (sx - 1 - y); // rotate 90° clockwise
	    
	    ((uint32_t*)rotref2.buffer)[dst_index] = ((uint32_t*)rotref1.buffer)[src_index];
	  }
	}

      }
    if(rot==90)

      {
	for (int y = 0; y < sy; y++) {
	  for (int x = 0; x < sx; x++) {
	    int src_index = y * sx + x;
	    int dst_index = x * sy + (sy - 1 - y); // rotate 90° clockwise
	    
	    ((uint32_t*)rotref1.buffer)[dst_index] = ((uint32_t*)ref.buffer)[src_index];
	  }
	}


	for (int y = 0; y < sx; y++) {
	  for (int x = 0; x < sy; x++) {
	    int src_index = y * sy + x;
	    int dst_index = x * sx + (sx - 1 - y); // rotate 90° clockwise
	    
	    ((uint32_t*)rotref2.buffer)[dst_index] = ((uint32_t*)rotref1.buffer)[src_index];
	  }
	}

	for (int y = 0; y < sy; y++) {
	  for (int x = 0; x < sx; x++) {
	    int src_index = y * sx + x;
	    int dst_index = x * sy + (sy - 1 - y); // rotate 90° clockwise
	    
	    ((uint32_t*)rotref3.buffer)[dst_index] = ((uint32_t*)rotref2.buffer)[src_index];
	  }
	}


      }
    int width=sx;
    int height=sy;
    if (rot==90||rot==270) std::swap(width,height);
    int ydelta = ref.ydelta;
    unsigned int *buf = ref.buffer;
    if(rot==270) { buf=rotref1.buffer; ydelta=rotref1.ydelta; }
    if(rot==180) { buf=rotref2.buffer; ydelta=rotref2.ydelta; }
    if(rot==90) { buf=rotref3.buffer; ydelta=rotref3.ydelta; }
    for(int y=0;y<height;y++)
      for(int x=0;x<width;x++)
	{
	  buf[x+y*ydelta]|=0xff000000; // alpha channel included
	}

    
    
    
    mediaBuffer->Unlock();
    mediaBuffer->Release();
    sample->Release();
    return true;
  }


  void Gen() const { }
  BufferRef Buffer() const {
    if (!init_done) {
      init();
      init_done = true;
    }
    update_frame();
    BufferRef *buf = &ref;
    if(rot==270) buf=&rotref1;
    if(rot==180) buf=&rotref2;
    if(rot==90) buf=&rotref3;
    return *buf;
  }  
private:
  std::string url;
  mutable int sx, sy;
  mutable IMFSourceReader* reader = nullptr;
  unsigned int tex;
  mutable bool init_done;
  mutable BufferRef ref;
  mutable BufferRef rotref1;
  mutable BufferRef rotref2;
  mutable BufferRef rotref3;
  mutable int rot=0;
};

#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>

EM_JS(void, webcam_cleanup, (void), {
    var video = document.getElementById('webcam');
    video.id = 'oldwebcam';
    video.remove();
    var video2 = document.getElementById('webcamCanvas');
    video2.id = 'oldwebcamCanvas';
    video2.remove();
  });

EM_JS(void, init_webcam, (const char *url, bool is_videofile, int sx, int sy), {
  const video = document.createElement('video');
  video.setAttribute('id', 'webcam');
  if (is_videofile)
    video.loop = true;
  video.autoplay = true;
  video.playsInline = true;
  if (is_videofile)
    video.crossOrigin = 'anonymous';
  if (is_videofile)
    video.src=UTF8ToString(url);
  else
    video.src='';
  video.style.display = 'none';
  video.width = sx;
  video.height = sy;
    video.muted = true;
  video.onloadedmetadata = () => {
    video.play().catch(err => console.error("Autoplay failed:", err));
  };
  document.body.appendChild(video);

  const canvas = document.createElement('canvas');
  canvas.setAttribute('id', 'webcamCanvas');
  canvas.setAttribute('willReadFrequently', 'true');
  canvas.width = sx;
  canvas.height = sy;
  canvas.style.display = 'none';
  document.body.appendChild(canvas);

  if (!is_videofile) {
  navigator.mediaDevices.getUserMedia({ video: true })
    .then((stream) => {
      video.srcObject = stream;
    })
    .catch((err) => {
      console.error('Webcam error:', err);
    });
  }
});

EM_JS(void, grab_frame_to_memory, (int dstPtr, int width, int height), {
  const video = document.getElementById('webcam');
  const heap = Module.HEAPU8;
  if ([undefined,null].includes(video) || video.readyState < 2) {
    // Not enough data yet
    for (let i = 0; i < width*height*4; ++i) {
      heap[dstPtr + i] = 0;
    }

    return;
  }
  const canvas = document.getElementById('webcamCanvas');
  const ctx = canvas.getContext('2d');
  ctx.drawImage(video, 0, 0, width, height);

  const imageData = ctx.getImageData(0, 0, width, height);
  const data = imageData.data;

  for (let i = 0; i < Math.min(data.length,width*height*4); ++i) {
    heap[dstPtr + i] = data[i];
  }
});

void emscripten_capture_cb(void*);

extern void (*g_stop_cb)(void*);
extern void *g_stop_cb_param;
extern bool g_stop_cb_enabled;


class EmscriptenCapture : public BufferRefReq
{
public:
  EmscriptenCapture(int sx, int sy, int num) : num(num),is_video_file(false),sx(sx),sy(sy) {
    //sx=640; sy=480;
    init_done = false;
    ref=BufferRef::NewBuffer(sx,sy);
    g_stop_cb = &emscripten_capture_cb;
    g_stop_cb_param = (void*)this;
    g_stop_cb_enabled = true;
  }
  EmscriptenCapture(int sx, int sy, std::string url) : url(url), num(0), is_video_file(true),sx(sx),sy(sy) { init_done = false; ref=BufferRef::NewBuffer(sx,sy);

    g_stop_cb = &emscripten_capture_cb;
    g_stop_cb_param = (void*)this;
    g_stop_cb_enabled = true;

    
  }
  ~EmscriptenCapture() { g_stop_cb_enabled = false; if (init_done) webcam_cleanup(); }
  void cleanup() { g_stop_cb_enabled = false; if (init_done) webcam_cleanup(); }
  void initCapture(int num) const
  {
    init_webcam(is_video_file?url.c_str():"",is_video_file,sx,sy);
  }
  void doCapture(int num) const
  {
    int sx = ref.width;
    int sy = ref.height;
    grab_frame_to_memory((int)ref.buffer,sx,sy);
    //std::copy(framebuffer,framebuffer+sx*sy*4,ref.buffer);
    for(int y=0;y<sy/2;y++)
      for(int x=0;x<sx;x++)
	{
	  std::swap(*(ref.buffer+x+y*ref.ydelta),*(ref.buffer+x+(ref.height-y-1)*ref.ydelta));
	}
    for(int y=0;y<sy;y++)
      for(int x=0;x<sx/2;x++)
	{
	  std::swap(*(ref.buffer+x+y*ref.ydelta),*(ref.buffer+(ref.width-x-1)+y*ref.ydelta));
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
  mutable std::string url;
  mutable uint8_t *framebuffer = nullptr;
  int sx,sy;
  mutable BufferRef ref;
  mutable int num;
  mutable bool init_done;
  mutable bool is_video_file;
};
void emscripten_capture_cb(void* p)
{
  EmscriptenCapture *cb2 = (EmscriptenCapture*)p;
  cb2->cleanup();
}


GameApi::TXID GameApi::TextureApi::webcam_txid_emscripten(EveryApi &ev, int sx, int sy, int num)
{
  EmscriptenCapture *buf = new EmscriptenCapture(sx,sy,num);
  WebCamBufferRefTexID *id = new WebCamBufferRefTexID(ev, *buf);
  return add_txid(e,id);
}
GameApi::TXID GameApi::TextureApi::videofile_txid_emscripten(EveryApi &ev, int sx, int sy, std::string url)
{
  EmscriptenCapture *buf = new EmscriptenCapture(sx,sy,url);
  WebCamBufferRefTexID *id = new WebCamBufferRefTexID(ev, *buf);
  return add_txid(e,id);
}




#endif

#ifdef WINDOWS
GameApi::TXID GameApi::TextureApi::videofile_txid_win32(EveryApi &ev, int sx, int sy, std::string url)
{
  VideoPlayer_win32 *buf = new VideoPlayer_win32(sx,sy,url);
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
#ifdef EMSCRIPTEN
  return webcam_txid_emscripten(ev,sx,sy,num);
#endif
}
GameApi::TXID GameApi::TextureApi::videofile_txid_generic(EveryApi &ev, int sx, int sy, std::string url)
{
#ifdef LINUX
  return ev.bitmap_api.video_source(url,sx,sy);
#endif
#ifdef EMSCRIPTEN
  return videofile_txid_emscripten(ev,sx,sy,url);
#endif
#ifdef WINDOWS
  return videofile_txid_win32(ev,sx,sy,url);
#endif
  
  GameApi::TXID id;
  id.id = 0;
  return id;
}
