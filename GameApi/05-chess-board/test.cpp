#include "GameApi.hh"

using namespace GameApi;

char chars[] =
  "THLKQLHT"
  "PPPPPPPP"
  "........"
  "........"
  "........"
  "........"
  "PPPPPPPP"
  "THLKQLHT";
int charsmap(char c)
{
  switch(c)
    {
    case 'T': return 0;
    case 'H': return 1;
    case 'L': return 2; 
    case 'K': return 3; 
    case 'Q': return 4;
    case 'P': return 5;
    }
}

P chars_blocks(int c, EveryApi &ev)
{
#if 0
  switch(c)
    {
    case 5: // pawn
      {
	PT p1 = ev.point_api.point(15.0, -60.0, 15.0);
	PT p2 = ev.point_api.point(15.0, -40.0, 15.0);
	P p = ev.polygon_api.cone(30, p1,p2,15.0,12.0);
	return p;
      }
    };
#endif
  return ev.polygon_api.empty();
}


char board[] = 
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX"
  "XOXOXOXO"
  "OXOXOXOX";

int boardmap(char c)
{
  switch(c)
    {
    case 'X': return 0;
    case 'O': return 1;
    };
}

P board_blocks(int c, EveryApi &ev)
{
  P block = ev.polygon_api.cube(0.0, 30.0,
				-60.0, -65.0,
				0.0, 30.0);
  P color_block;
  switch(c) {
  case 0:
    color_block = ev.polygon_api.color_faces(block, 0xffffffff, 0xaaaaaaff, 0xccccccff, 0x888888ff);
    break;
  case 1:
    color_block = ev.polygon_api.color_faces(block, 0x000000ff, 0x222222ff, 0x444444ff, 0x111111ff);
  };
  return color_block;
}

int main() {
  Env e;
  EveryApi ev(e);

  // initialize window
  ev.mainloop_api.init_window();

  // shader initialization
  ev.shader_api.load("Shader.txt");
  SH sh = ev.shader_api.colour_shader();

  // rest of the initializations
  ev.mainloop_api.init_3d(sh);

  BM bm = ev.bitmap_api.newintbitmap(board, 8,8, boardmap);
  P board = ev.polygon_api.world_from_bitmap(std::bind(&board_blocks, _1, std::ref(ev)), bm, 30.0, 30.0);
  P board2 = ev.polygon_api.scale(board, 2.8,2.8,2.8);
  P board3 = ev.polygon_api.translate(board2, -340.0, 30.0, -400.0);

  BM bm2 = ev.bitmap_api.newintbitmap(chars, 8,8, charsmap);
  P chars = ev.polygon_api.world_from_bitmap(std::bind(&chars_blocks, _1, std::ref(ev)), bm2, 30.0, 30.0);
  P chars2 = ev.polygon_api.scale(chars, 2.8,2.8,2.8);
  P chars3 = ev.polygon_api.translate(chars2, -340.0, 30.0, -400.0);

  P or_b = ev.polygon_api.or_elem(board3, chars3);

  PolygonObj poly(ev, or_b, sh);
  poly.prepare();
 
  while(1) {
    // clear frame buffer
    ev.mainloop_api.clear_3d();

    poly.render();

    ev.mainloop_api.fpscounter();
    // swapbuffers
    ev.mainloop_api.swapbuffers();

    // handle esc event
    MainLoopApi::Event e = ev.mainloop_api.get_event();
    if (e.ch==27) break;
  }



}
