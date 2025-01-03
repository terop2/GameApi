
#include "GameApi.hh"
//#pragma comment(lib, "d:\\cvs\\gameapi-code\\GameApi\\GameApi\\Debug\\GameApi\\GameApi.lib")
#include <iostream>

using namespace GameApi;

void GameTest9(GameApi::EveryApi &e)
{
  e.mainloop_api.init_window();
  e.shader_api.load("../Shader.txt");
  SH sh = e.shader_api.get_shader("texture", "texture", "");
  e.shader_api.bind_attrib(sh, 0, "in_Position");
  e.shader_api.bind_attrib(sh, 1, "in_Normal");
  e.shader_api.bind_attrib(sh, 2, "in_Color");
  e.shader_api.bind_attrib(sh, 3, "in_TexCoord");
  e.shader_api.link(sh);

  e.mainloop_api.init(sh);
  
#if 0
  BM bm = e.bitmap_api.loadbitmap("../pics/Sisäkuvia_001 012.jpg");
  BM bm2 = e.bitmap_api.loadbitmap("../pics/Sisäkuvia_001 013.jpg");
  BM bm3 = e.bitmap_api.loadbitmap("../pics/Sisäkuvia_001 018.jpg");
#endif
#if 1
  // tiedotus ja hallinto
  BM bm = e.bitmap_api.loadbitmap("../pics2/IMG_0022.JPG");
  BM bm3 = e.bitmap_api.loadbitmap("../pics2/IMG_0002.JPG");
  BM bm2 = e.bitmap_api.loadbitmap("../pics2/IMG_0016.JPG");
#endif
#if 0
  // tiedotus ja hallinto #2
  BM bm = e.bitmap_api.loadbitmap("../pics2/IMG_0016.JPG");
  BM bm2 = e.bitmap_api.loadbitmap("../pics2/IMG_0017.JPG");
  BM bm3 = e.bitmap_api.loadbitmap("../pics2/IMG_0019.JPG");
#endif
#if 0
  // kaytava
  BM bm = e.bitmap_api.loadbitmap("../pics2/IMG_0005.JPG");
  BM bm2 = e.bitmap_api.loadbitmap("../pics2/IMG_0006.JPG");
  BM bm3 = e.bitmap_api.loadbitmap("../pics2/IMG_0010.JPG");
#endif
#if 0
  // keittio
  BM bm = e.bitmap_api.loadbitmap("../pics2/IMG_0015.JPG");
  BM bm2 = e.bitmap_api.loadbitmap("../pics2/IMG_0018.JPG");
  BM bm3 = e.bitmap_api.loadbitmap("../pics2/IMG_0008.JPG");
#endif
#if 0
  // työ ja koulutus
  BM bm = e.bitmap_api.loadbitmap("../pics2/IMG_0013.JPG");
  BM bm2 = e.bitmap_api.loadbitmap("../pics2/IMG_0011.JPG");
  BM bm3 = e.bitmap_api.loadbitmap("../pics2/IMG_0012.JPG");
#endif

#if 0
  // random
  BM bm = e.bitmap_api.loadbitmap("../pics2/IMG_0009.JPG");
  BM bm2 = e.bitmap_api.loadbitmap("../pics2/IMG_0013.JPG");
  BM bm3 = e.bitmap_api.loadbitmap("../pics2/IMG_0014.JPG");
#endif


  //  BM bm_used = bm2;
  float time = 0.0;
  
  std::pair<BM,BM> p1 = std::make_pair(bm,bm2);
  std::pair<BM,BM> p2 = std::make_pair(bm2,bm3);
  std::pair<BM,BM> p3 = std::make_pair(bm3,bm);

  std::pair<BM,BM> array[] = { p1, p2 , p3 };
  //int frame = 0;
  while(1)
    {
      e.mainloop_api.clear();
      int num = 0;
      float val = time*50.0;
      while(val>100.0*3) val-=100.0*3;
      float val2 = val;
      while(val2>100.0) { num++; val2-=100.0; }
      
      while(val>100.0) { val-=100.0; }
      if (val<80.0) { val = 0.0; } else
	{
	  val-=80.0;
	  val*=5.0;
	}
      //std::cout << num << std::endl;
      std::pair<BM,BM> p = array[num];
      val/=100.0;
      BM mix = e.bitmap_api.interpolate_bitmap(p.first, p.second, val);
      //e.sprite_api.preparesprite(mix);
      //e.sprite_api.rendersprite(mix, sh, 0.0, 0.0, 0.5,0.5);

      CBM mix2 = e.cont_bitmap_api.from_bitmap(mix, 1.0, 1.0);
      BM mix22 = e.cont_bitmap_api.sample(mix2, e.bitmap_api.size_x(mix)/8, e.bitmap_api.size_y(mix)/8);
      //BM mix22 = e.cont_bitmap_api.to_bitmap(mix2, e.bitmap_api.size_x(mix)/8, e.bitmap_api.size_y(mix)/8);
      e.sprite_api.preparesprite(mix22);
      e.sprite_api.rendersprite(mix22, sh, 0.0, 0.0, 1.0,1.0);
#if 0
      std::string filename = "scr.bmp";
      frame++;
      std::string lead = "000";
      lead[0] = frame /100;
      lead[1] = (frame-lead[0]*100)/10;
      lead[2] = (frame-lead[0]*100-lead[1]*10);
      lead[0]+='0';
      lead[1]+='0';
      lead[2]+='0';
      e.bitmap_api.savebitmap(mix22, lead+filename);
#endif

      e.mainloop_api.swapbuffers();
      MainLoopApi::Event ev = e.mainloop_api.get_event();
      if (ev.ch==27) break;
      time += 0.05;
    }
}
