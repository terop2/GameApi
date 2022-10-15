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

#ifndef HTML5_MUSIC_H_
#define HTML5_MUSIC_H_

#include "prerequisites.h"

#ifndef HTML5_MIXER
#define HTML5_MIXER
#endif

#ifndef MUSIC_HTML5
#define MUSIC_HTML5
#endif

typedef enum
{
	MIX_MUSIC_HTML5,
	MIX_MUSIC_CMD,
	MIX_MUSIC_WAVE,
	MIX_MUSIC_MODPLUG,
	MIX_MUSIC_MIKMOD,
	MIX_MUSIC_FLUIDSYNTH,
	MIX_MUSIC_TIMIDITY,
	MIX_MUSIC_NATIVEMIDI,
	MIX_MUSIC_OGG,
	MIX_MUSIC_MPG123,
	MIX_MUSIC_MAD,
	MIX_MUSIC_SMPEG,
	MIX_MUSIC_FLAC,
	MIX_MUSIC_LAST
} Mix_MusicAPI;


/* Music API implementation */

typedef struct
{
	const char *tag;
	Mix_MusicAPI api;
	Mix_MusicType type;
	SDL_bool loaded;
	SDL_bool opened;

	/* Load the library */
	int (*Load)(void);

	/* Initialize for the audio output */
	int (*Open)(const SDL_AudioSpec *spec);

	/* Create a music object from an SDL_RWops stream
	 * If the function returns NULL, 'src' will be freed if needed by the caller.
	 */
	void *(*CreateFromRW)(SDL_RWops *src, int freesrc);

	/* Create a music object from a file, if SDL_RWops are not supported */
	void *(*CreateFromFile)(const char *file);

	/* Set the volume */
	void (*SetVolume)(void *music, int volume);

	/* Start playing music from the beginning with an optional loop count */
	int (*Play)(void *music, int play_count);

	/* Returns SDL_TRUE if music is still playing */
	SDL_bool (*IsPlaying)(void *music);

	/* Get music data, returns the number of bytes left */
	int (*GetAudio)(void *music, void *data, int bytes);

	/* Seek to a play position (in seconds) */
	int (*Seek)(void *music, double position);

	/* Pause playing music */
	void (*Pause)(void *music);

	/* Resume playing music */
	void (*Resume)(void *music);

	/* Stop playing music */
	void (*Stop)(void *music);

	/* Delete a music object */
	void (*Delete)(void *music);

	/* Close the library and clean up */
	void (*Close)(void);

	/* Unload the library */
	void (*Unload)(void);

} Mix_MusicInterface;

struct _Mix_Music {
	Mix_MusicInterface *interface;
	void *context;

	SDL_bool playing;
	Mix_Fading fading;
	int fade_step;
	int fade_steps;
};

extern void run_music_finished_hook(void);

#endif // #ifndef HTML5_MUSIC_H_
