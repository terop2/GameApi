
#include <assert.h>
#include <stdio.h>
#ifdef EMSCRIPTEN
#include <emscripten/em_math.h>
#include "lib/lib_webgpu.h"
#endif
#include "GameApi_h.hh"

#ifdef EMSCRIPTEN

double hue2color(double hue)
{
  hue = emscripten_math_fmod(hue, 1.0);
  if (hue < 1.0 / 6.0) return 6.0 * hue;
  if (hue < 1.0 / 2.0) return 1;
  if (hue < 2.0 / 3.0) return 4.0 - 6.0 * hue;
  return 0;
}




EM_JS(void, js_show_gpu_canvas, (), {
    var cnv1 = document.getElementById("canvas");
    var cnv2 = document.getElementById("gpucanvas");
    cnv2.style="";
    cnv1.style="display:none";
  });
EM_JS(void, js_hide_gpu_canvas, (), {
    var cnv1 = document.getElementById("canvas");
    var cnv2 = document.getElementById("gpucanvas");
    cnv2.style="display:none";
    cnv1.style="";
  });

class WebGpuWindow : public Splitter
{
public:
  WebGpuWindow(MainLoopItemWGPU *item) : item(item) { }

  bool NoMainLoop() const { return true; } // disable mainloop
  void show_canvas( bool b )
  {
    if (b) { js_show_gpu_canvas(); } else { js_hide_gpu_canvas(); }
  }

  
  EMSCRIPTEN_KEEPALIVE static EM_BOOL raf(double time, void *userData)
  {
    WebGpuWindow *win = m_this;
    //WebGpuWindow *win = (WebGpuWindow*)userData;
    WGpuCommandEncoder encoder = wgpu_device_create_command_encoder_simple(win->device);
    
    WGpuRenderPassColorAttachment colorAttachment = WGPU_RENDER_PASS_COLOR_ATTACHMENT_DEFAULT_INITIALIZER;
    colorAttachment.view = wgpu_canvas_context_get_current_texture_view(win->canvasContext);

    double hue = time * 0.00005;
    colorAttachment.clearValue.r = hue2color(hue + 1.0 / 3.0);
    colorAttachment.clearValue.g = hue2color(hue);
    colorAttachment.clearValue.b = hue2color(hue - 1.0 / 3.0);
    colorAttachment.clearValue.a = 1.0;
    colorAttachment.loadOp = WGPU_LOAD_OP_CLEAR;
    
    WGpuRenderPassDescriptor passDesc = {};
    passDesc.numColorAttachments = 1;
    passDesc.colorAttachments = &colorAttachment;
    
    wgpu_render_pass_encoder_end(wgpu_command_encoder_begin_render_pass_1color_0depth(encoder, &passDesc));
    wgpu_queue_submit_one_and_destroy(win->queue, wgpu_command_encoder_finish(encoder));


  static int numLiveObjects = 0;
  int numLiveNow = wgpu_get_num_live_objects();
  if (numLiveNow != numLiveObjects)
  {
    std::cout << "Num live WebGPU objects: " << numLiveNow << std::endl;
    numLiveObjects = numLiveNow;
  }

    
  //  assert(wgpu_get_num_live_objects() < 100); // Check against programming errors from Wasm<->JS WebGPU object leaks

    return EM_TRUE;
  }
  
  EMSCRIPTEN_KEEPALIVE static void ObtainedWebGpuDevice(WGpuDevice result, void *userData)
  {
    WebGpuWindow *win = m_this;
    win->device = result;
    win->queue = wgpu_device_get_queue(win->device);
    
    win->canvasContext = wgpu_canvas_get_webgpu_context("#gpucanvas");
    
    WGpuCanvasConfiguration config = WGPU_CANVAS_CONFIGURATION_DEFAULT_INITIALIZER;
    config.device = win->device;
    config.format = navigator_gpu_get_preferred_canvas_format();
    wgpu_canvas_context_configure(win->canvasContext, &config);

    emscripten_request_animation_frame_loop(raf, 0);
  }
  
  
  EMSCRIPTEN_KEEPALIVE static void ObtainedWebGpuAdapter(WGpuAdapter result, void *userData)
  {
    WebGpuWindow *win = m_this;
    win->adapter = result;
    wgpu_adapter_request_device_async_simple(win->adapter, ObtainedWebGpuDevice);
  }

  EMSCRIPTEN_KEEPALIVE virtual void Init()
  {
    m_this = this;
    //assert(navigator_gpu_available());
    navigator_gpu_request_adapter_async_simple(ObtainedWebGpuAdapter);
    show_canvas(true);
  }
  EMSCRIPTEN_KEEPALIVE virtual int Iter()
  {
    //EM_BOOL b = raf(0.0 /* time */, (void*)this);
    return -1;
  }
  virtual void Destroy()
  {
    show_canvas(false);
  }
private:
  static WebGpuWindow *m_this;
  WGpuAdapter adapter;
  WGpuDevice device;
  WGpuQueue queue;
  WGpuCanvasContext canvasContext;
  MainLoopItemWGPU *item;
};
WebGpuWindow *WebGpuWindow::m_this;
#endif



GameApi::RUN GameApi::BlockerApi::webgpu_window(GameApi::EveryApi &ev, GML ml)
{
#ifdef EMSCRIPTEN
  MainLoopItemWGPU *gpu = find_main_loop_wgpu(e,ml);
  Splitter *spl = new WebGpuWindow(gpu);
  return add_splitter(e, spl);
#endif
#ifndef EMSCRIPTEN
  std::cout << "WEBGPU_WINDOW not supported by linux/win32 builds." << std::endl;
  exit(0);
#endif
}

class RenderTriangles : public MainLoopItemWGPU
{
public:
  RenderTriangles(FaceCollection *coll) : coll(coll) { }
  virtual void Collect(CollectVisitor &vis) { }
  virtual void HeavyPrepare() { }
  virtual void execute(MainLoopEnv &e) { }
  virtual void handle_event(MainLoopEvent &e)
  {
  }
  virtual std::vector<int> shader_id() { return std::vector<int>(); }

private:
  FaceCollection *coll;
};
