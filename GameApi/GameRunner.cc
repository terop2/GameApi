
#define SDL2_USED
#define GAME_API_DEFS
#define _SCL_SECURE_NO_WARNINGS


#include "GameRunner.hh"
#include "GameApi.hh"

#if 0
#define NO_SDL_GLEXT
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#ifdef SDL2_USED
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include <SDL/SDL.h> 
#include <SDL/SDL_opengl.h>
#endif
#endif
#include "GameApi_low.hh"

extern Low_SDL_Window *sdl_window;

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
	g_low->ogl->glLoadIdentity();
#endif
	g_low->sdl->SDL_GL_SwapWindow(sdl_window);
	//SDL_GL_SwapBuffers();
#if 0
	SDL_Event event;
	SDL_PollEvent(&event);
#endif
	time+=30.0;
      }
}
