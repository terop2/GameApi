// html5_mixer
//
// Copyright (c) 2021 David Apollo (77db70f775fa0b590889c45371a70a1d23e99869d4565976a5207c11606fb6aa)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include "html5_mixer.h"
#include "music_html5.h"

static Mix_Music *music_playing;
static SDL_bool music_active = SDL_TRUE;
static void (SDLCALL *music_finished_hook)(void) = NULL;

////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////

int HTML5_Mix_Init(int flags)
{
	// In SDL Mixer, this happens in Mix_OpenAudio().
	// We don't shim that, so HACK: do it here.

	Mix_MusicInterface_HTML5.Open(0);
	Mix_MusicInterface_HTML5.loaded = SDL_TRUE;

	return flags;
}

/* Unloads libraries loaded with Mix_Init */
void HTML5_Mix_Quit(void)
{
	// In SDL Mixer, this happens in Mix_CloseAudio().
	// We don't shim that, so HACK: do it here.

	if (music_playing)
		HTML5_Mix_HaltMusic();

	Mix_MusicInterface_HTML5.Close();
}

////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////

/* Load a music file */
Mix_Music *HTML5_Mix_LoadMUS(const char *file)
{
	void *context = Mix_MusicInterface_HTML5.CreateFromFile(file);

	if (context)
	{
		Mix_Music *music = (Mix_Music *)SDL_calloc(1, sizeof(Mix_Music));
		if (music == NULL) {
			Mix_SetError("Out of memory");
			return NULL;
		}
		music->interface = &Mix_MusicInterface_HTML5;
		music->context = context;
		return music;
	}

	// SDL Mixer attempts to load by RW after first trying the file path.
	// Our HTML5 interface already does this, so don't take additional action.

	return NULL;
}

Mix_Music *HTML5_Mix_LoadMUS_RW(SDL_RWops *src, int freesrc)
{
	return HTML5_Mix_LoadMUSType_RW(src, MUS_NONE, freesrc);
}

Mix_Music *HTML5_Mix_LoadMUSType_RW(SDL_RWops *src, Mix_MusicType type, int freesrc)
{
	void *context = Mix_MusicInterface_HTML5.CreateFromRW(src, freesrc);

	if (context)
	{
		Mix_Music *music = (Mix_Music *)SDL_calloc(1, sizeof(Mix_Music));
		if (music == NULL) {
			Mix_SetError("Out of memory");
			return NULL;
		}
		music->interface = &Mix_MusicInterface_HTML5;
		music->context = context;
		return music;
	}

	return NULL;
}

void HTML5_Mix_FreeMusic(Mix_Music *music)
{
	if (music_playing == music)
		HTML5_Mix_HaltMusic();

	// TODO: Wait for any fade out to finish
	Mix_MusicInterface_HTML5.Delete(music->context);
	
	SDL_free(music);
}

////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////

void HTML5_Mix_HookMusicFinished(void (SDLCALL *music_finished)(void))
{
	music_finished_hook = music_finished;
}

void run_music_finished_hook(void)
{
	// Reset music status to default. In SDL Mixer, this is handled in
	// the mix_music() callback loop. For HTML5 Mixer, we handle here because we are
	// asynchronously called from an <audio> event handler "ended".
	music_playing = NULL;
	music_active = SDL_TRUE;

	if (music_finished_hook)
		music_finished_hook();
}

////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////

/* Play a music chunk.  Returns 0, or -1 if there was an error.
 */
int HTML5_Mix_FadeInMusicPos(Mix_Music *music, int loops, int ms, double position)
{
	int retval;
	(void)ms;

	// Clean up after old music
	if (music_playing)
		HTML5_Mix_HaltMusic();

	// TODO: Fade
	if (position)
		Mix_MusicInterface_HTML5.Seek(music->context, position);

	retval = Mix_MusicInterface_HTML5.Play(music->context, loops);

	music_playing = music;
	music_playing->playing = SDL_TRUE;
	music_active = (retval == 0);

	return 0;
}
int HTML5_Mix_FadeInMusic(Mix_Music *music, int loops, int ms)
{
	return HTML5_Mix_FadeInMusicPos(music, loops, ms, 0.0);
}
int HTML5_Mix_PlayMusic(Mix_Music *music, int loops)
{
	return HTML5_Mix_FadeInMusicPos(music, loops, 0, 0.0);
}

////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////

/* Check the status of the music */
int HTML5_Mix_PlayingMusic(void)
{
	return music_playing ? Mix_MusicInterface_HTML5.IsPlaying(music_playing->context) : SDL_FALSE;
}

static int music_volume = SDL_MIX_MAXVOLUME;

/* Set the music volume */
int HTML5_Mix_VolumeMusic(int volume)
{
	int prev_volume = SDL_MIX_MAXVOLUME;

	// TODO: Retrieve prev_volume from <audio>
	if (music_playing)
		Mix_MusicInterface_HTML5.SetVolume(music_playing->context, volume);

	return(prev_volume);
}

/* Halt playing of music */
int HTML5_Mix_HaltMusic(void)
{
	if (music_playing)
	{
		music_playing->interface->Stop(music_playing->context);
		music_playing->playing = SDL_FALSE;
		music_playing = NULL;

		run_music_finished_hook();
	}
	else
	{
		Mix_SetError("Music isn't playing");
		return -1;
	}
	return(0);
}

void HTML5_Mix_PauseMusic(void)
{
	if (music_playing)
		music_playing->interface->Pause(music_playing->context);
	music_active = SDL_FALSE;
}

void HTML5_Mix_ResumeMusic(void)
{
	if (music_playing)
		music_playing->interface->Resume(music_playing->context);
	music_active = SDL_TRUE;
}

SDL_bool HTML5_Mix_PausedMusic(void)
{
	return (music_active == SDL_FALSE);
}

/* Set the playing music position */
int HTML5_Mix_SetMusicPosition(double position)
{
	if (music_playing)
		music_playing->interface->Seek(music_playing->context, position);
	else
	{
		Mix_SetError("Music isn't playing");
		return -1;
	}
	return(0);
}
