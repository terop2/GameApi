
#define SDL2_USED
#define GAME_API_DEFS
#define _SCL_SECURE_NO_WARNINGS


#include "GameRunner.hh"
#include "GameApi.hh"

#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <GL/gl.h>
#ifdef SDL2_USED
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include <SDL/SDL.h> 
#include <SDL/SDL_opengl.h>
#endif

extern SDL_Window *sdl_window;

void GameRunner::run()
{
    Alloc();
    current_state = start_state;
    float time = 0.0;
    while(1)
      {
	current_time = time;
	current_state = MoveToNextState(current_state, time);
	RenderState(current_state);

#ifndef EMSCRIPTEN
	glLoadIdentity();
#endif
	SDL_GL_SwapWindow(sdl_window);
	//SDL_GL_SwapBuffers();

	SDL_Event event;
	SDL_PollEvent(&event);

	time+=30.0;
      }
}
