#ifndef HTML5_MIXER_H_
#define HTML5_MIXER_H_

#include <emscripten.h>
#include "prerequisites.h"

////////////////////////////////////////////////////////////////////////
// Mixer Function Shims
////////////////////////////////////////////////////////////////////////

#ifdef HTML5_MIXER_SHIM_MUSIC
#define Mix_Init HTML5_Mix_Init
#define Mix_Quit HTML5_Mix_Quit
#define Mix_LoadMUS HTML5_Mix_LoadMUS
#define Mix_LoadMUS_RW HTML5_Mix_LoadMUS_RW
#define Mix_LoadMUSType_RW HTML5_Mix_LoadMUSType_RW
#define Mix_LoadMUS_RW HTML5_Mix_LoadMUS_RW
#define Mix_FreeMusic HTML5_Mix_FreeMusic
#define Mix_HookMusicFinished HTML5_Mix_HookMusicFinished
#define Mix_PlayMusic HTML5_Mix_PlayMusic
#define Mix_FadeInMusic HTML5_Mix_FadeInMusic
#define Mix_FadeInMusicPos HTML5_Mix_FadeInMusicPos
#define Mix_PlayingMusic HTML5_Mix_PlayingMusic
#define Mix_VolumeMusic HTML5_Mix_VolumeMusic
#define Mix_HaltMusic HTML5_Mix_HaltMusic
#define Mix_PauseMusic HTML5_Mix_PauseMusic
#define Mix_ResumeMusic HTML5_Mix_ResumeMusic
#define Mix_PausedMusic HTML5_Mix_PausedMusic
#define Mix_SetMusicPosition HTML5_Mix_SetMusicPosition
#endif

////////////////////////////////////////////////////////////////////////
// Function Definitions
////////////////////////////////////////////////////////////////////////

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Loads dynamic libraries and prepares them for use.  Flags should be
   one or more flags from MIX_InitFlags OR'd together.
   It returns the flags successfully initialized, or 0 on failure.
 */
extern DECLSPEC int SDLCALL HTML5_Mix_Init(int flags);

/* Unloads libraries loaded with Mix_Init */
extern DECLSPEC void SDLCALL HTML5_Mix_Quit(void);

/* Load a wave file or a music (.mod .s3m .it .xm) file */
extern DECLSPEC Mix_Music * SDLCALL Mix_LoadMUS(const char *file);

/* Load a music file from an SDL_RWop object (Ogg and MikMod specific currently)
   Matt Campbell (matt@campbellhome.dhs.org) April 2000 */
extern DECLSPEC Mix_Music * SDLCALL HTML5_Mix_LoadMUS_RW(SDL_RWops *src, int freesrc);

/* Load a music file from an SDL_RWop object assuming a specific format */
extern DECLSPEC Mix_Music * SDLCALL HTML5_Mix_LoadMUSType_RW(SDL_RWops *src, Mix_MusicType type, int freesrc);

/* Free an audio chunk previously loaded */
extern DECLSPEC void SDLCALL HTML5_Mix_FreeMusic(Mix_Music *music);

/* Add your own callback for when the music has finished playing or when it is
 * stopped from a call to Mix_HaltMusic.
 */
extern DECLSPEC void SDLCALL HTML5_Mix_HookMusicFinished(void (SDLCALL *music_finished)(void));

/* Play an audio chunk on a specific channel.
   If 'loops' is greater than zero, loop the sound that many times.
   If 'loops' is -1, loop inifinitely (~65000 times).
   Returns which channel was used to play the sound.
*/
extern DECLSPEC int SDLCALL HTML5_Mix_PlayMusic(Mix_Music *music, int loops);

/* Fade in music or a channel over "ms" milliseconds, same semantics as the "Play" functions */
extern DECLSPEC int SDLCALL HTML5_Mix_FadeInMusic(Mix_Music *music, int loops, int ms);
extern DECLSPEC int SDLCALL HTML5_Mix_FadeInMusicPos(Mix_Music *music, int loops, int ms, double position);

/* Check the status of a specific channel.
   If the specified channel is -1, check all channels.
*/
extern DECLSPEC int SDLCALL HTML5_Mix_PlayingMusic(void);

/* Set the volume in the range of 0-128 of a specific channel or chunk.
   If the specified channel is -1, set volume for all channels.
   Returns the original volume.
   If the specified volume is -1, just return the current volume.
*/
extern DECLSPEC int SDLCALL HTML5_Mix_VolumeMusic(int volume);

/* Halt playing of a particular channel */
extern DECLSPEC int SDLCALL HTML5_Mix_HaltMusic(void);

/* Pause/Resume the music stream */
extern DECLSPEC void SDLCALL HTML5_Mix_PauseMusic(void);
extern DECLSPEC void SDLCALL HTML5_Mix_ResumeMusic(void);

/* Set the current position in the music stream.
   This returns 0 if successful, or -1 if it failed or isn't implemented.
   This function is only implemented for MOD music formats (set pattern
   order number) and for OGG, FLAC, MP3_MAD, MP3_MPG and MODPLUG music
   (set position in seconds), at the moment.
*/
extern DECLSPEC int SDLCALL HTML5_Mix_SetMusicPosition(double position);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif // #ifndef HTML5_MIXER_H_
