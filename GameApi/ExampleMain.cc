#include "GameApi.hh"
#pragma comment(lib, "d:\\cvs\\gameapi-code\\GameApi\\GameApi\\Debug\\GameApi\\GameApi.lib")
#include <iostream>
#ifdef WIN32
#include <Windows.h>
#endif
void GameApiTest(GameApi::EveryApi &e);
void GameApiTest2(GameApi::EveryApi &e);
void GameApiTest3(GameApi::EveryApi &e);
void Game(GameApi::EveryApi &e);
void GameApiWorldFunc(GameApi::EveryApi &e);
void GameTest5(GameApi::EveryApi &e);
void GameTest6(GameApi::EveryApi &e);
void GameTest7(GameApi::EveryApi &e);
void GameTest8(GameApi::EveryApi &e);
void GameTest9(GameApi::EveryApi &e);
void GameTestA(GameApi::EveryApi &e);

int main(int argc, char *argv[])
{

  if (argc>1 && std::string(argv[1])=="-world")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameApiWorldFunc);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  else if ((argc>1 && std::string(argv[1])=="-test4"))
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &Game);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  else if (argc>1 && std::string(argv[1])=="-test5")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameTest5);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  else if (argc>1 && std::string(argv[1])=="-test6")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameTest6);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  else if (argc>1 && std::string(argv[1])=="-test7")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameTest7);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  else if (argc>1 && std::string(argv[1])=="-test8")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameTest8);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  else if (argc>1 && std::string(argv[1])=="-test9")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameTest9);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }
  else if (argc>1 && std::string(argv[1])=="-testa")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameTestA);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }

  else if (argc>1 && std::string(argv[1])=="-test3")
    {
      GameApi::Env e;
      GameApi::GamesApi g(e);
      g.register_game(0, &GameApiTest3);
      g.modify_map(0,0);
      g.start_game(0);
      return 0;
    }

  std::cout << "./main [-h] [-edit filename] [-test3] [-test4] [-world]" << std::endl;
}
