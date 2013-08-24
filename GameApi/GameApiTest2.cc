
#include "GameApi.hh"
#include <vector>
#include <iostream>

using namespace GameApi;

void GameApiTest2(EveryApi &every)
{
  Env e;
  MainLoopApi loop(e);
  BitmapApi bm(e);
  //VBO vbo(e);
  PolygonApi poly(e);
  EventApi event(e);

  loop.init();

  BM mand = bm.mandelbrot(false, -2.0, 1.0, -1.0, 1.0,
			  0.0, 0.0,
			  5,5,
			  128);
  BM mand2 = bm.mandelbrot(false, -1.0, 1.0, -1.0, 1.0,
			   0.0, 0.0,
			   300,200,
			   128);
  std::vector<P> vec;
  int i =0;
  for (float x=100.0;x<150.0;x+=2.0)
    {
      P quad = poly.quad_z(0.0, 100.0,
			   0.0, x,
			   0.0);
      P texquad = poly.texture(quad, mand);
      
      P quad2 = poly.quad_z(0.0+200.0, 100.0,
			    0.0+200.0, x,
			    0.0);
      P texquad2 = poly.texture(quad2, mand2);
      
      P pp = poly.or_elem(texquad, texquad2);
      P mm = poly.memoize(pp);
      
      P res2 = mm;
      vec.push_back(res2); i++;
    }
  P res = poly.anim_array(&vec[0],i); 
  E root = event.root_event();
  E end = event.timer(root, 1000.0);
  L poly2 = event.polygon(root, end, res);
  MV pn = event.point(0.0,0.0,0.0);
  MV pn2 = event.point(100.0,100.0,0.0);
  MV pn3 = event.point(0.0,0.0,100.0);
  MV pn4 = event.point(100.0,100.0,100.0);
  MV line = event.line(root, end, pn, pn2, 0, 0);
  MV line2 = event.line(root, end, pn3, pn4, 0,0);
  MV lineline = event.line(root, end, line, line2, 0,0);
  LL polypos = event.link(poly2, lineline);
  ST st = event.states(1);
  ST en = event.enable_obj(st, 0, polypos);
  en = en;
  //std::string s = event.Serialize(en, 0);
  //std::cout << s << std::endl;
  //event.run_game(en, 0);
#if 0
  poly.prepare(res);
#if 0
  P hm = poly.heightmap(mand, 
			0.0, 300.0,
  			0.0, 200.0,
  			0.0, 300.0);
  P hm2 = poly.memoize(hm);
  poly.prepare(hm2);
#endif
  int frame = 0;
  while(1)
    {
      frame++;
      //loop.switch_to_3d(true);
      loop.clear();
      poly.render(res, frame %i, 0.0,0.0,0.0);
      //poly.render(hm2, 0, 0.0,0.0,0.0);
      loop.swapbuffers();
      MainLoop::Event e = loop.get_event();
      if (e.ch==27) break;
    }
#endif  

}
