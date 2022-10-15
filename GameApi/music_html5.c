/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2021 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* This file supports an external command for playing music */

#include "music_html5.h"

#ifdef MUSIC_HTML5

#include <emscripten.h>

#ifdef HTML5_MIXER
// html5_mixer is a minimal implementation of SDL Mixer that supports
// only the HTML5 <audio> output.
//
// See https://github.com/devappd/html5_mixer

#define SDL_MIXER_HTML5_DISABLE_TYPE_CHECK (SDL_TRUE)

#ifdef HTML5_MIXER_ALLOW_AUTOPLAY
#define SDL_MIXER_HTML5_ALLOW_AUTOPLAY (SDL_TRUE)
#else
#define SDL_MIXER_HTML5_ALLOW_AUTOPLAY (SDL_FALSE)
#endif

#else
#define SDL_MIXER_HTML5_DISABLE_TYPE_CHECK (SDL_GetHint("SDL_MIXER_HTML5_DISABLE_TYPE_CHECK") ? SDL_TRUE : SDL_FALSE)
#define SDL_MIXER_HTML5_ALLOW_AUTOPLAY (SDL_GetHint("SDL_MIXER_HTML5_ALLOW_AUTOPLAY") ? SDL_TRUE : SDL_FALSE)
#endif

typedef struct {
    int id;
    SDL_RWops *src;
    SDL_bool freesrc;
    SDL_bool playing;
} MusicHTML5;

static SDL_bool html5_opened(void)
{
    return EM_ASM_INT({
        return !!Module["SDL2Mixer"] && !!Module["SDL2Mixer"].music;
    });
}

static void html5_handle_music_stopped(void *context)
{
    // Sets music->playing to FALSE. Call "finished" handler explicitly
    // in devappd/html5_mixer which does not run its own sound loop.

    MusicHTML5 *music = (MusicHTML5 *)context;
    if (music)
        music->playing = SDL_FALSE;

#ifdef HTML5_MIXER
    run_music_finished_hook();
#endif
}

static int MusicHTML5_Open(const SDL_AudioSpec *spec)
{
    (void)spec;

    if (html5_opened())
        return 0;

    EM_ASM(({
        const wasmMusicStopped = $0;
        const allowAutoplay = $1;

        Module["SDL2Mixer"] = {
            ////////////////////////////////////////////////////////////
            // Data
            ////////////////////////////////////////////////////////////

            player: (function() {
                const newPlayer = new Audio();
                // TODO: Make this configurable
                newPlayer.crossOrigin = 'anonymous';
                return newPlayer;
            })(),

            blob: {
                // URL.createObjectURL(...): numUses (int)
            },

            music: {
                // randomId: {
                //     src: (str),
                //     context: (int),
                //     playCount: (int),
                //     volume: (int)
                // };
            },

            ////////////////////////////////////////////////////////////
            // player <-> music management
            ////////////////////////////////////////////////////////////

            setPlayerProperty: function (id, property, value) {
                this.music[id][property] = value;
                if (this.player.dataset.currentId == id)
                    this.player[property] = value;
            },

            setPlayerDatasetProperty: function (id, property, value) {
                // music objects do not differentiate dataset fields
                this.music[id][property] = value;
                if (this.player.dataset.currentId == id)
                    this.player.dataset[property] = value;
            },

            setPlayerVolume: function(id, volume) {
                this.setPlayerProperty(id, "volume", volume);
            },

            setPlayerLoop: function(id, loop) {
                this.setPlayerProperty(id, "loop", loop);
            },

            setPlayerCurrentTime: function(id, currentTime) {
                this.setPlayerProperty(id, "currentTime", currentTime);
            },

            setPlayerPlayCount: function(id, playCount) {
                this.setPlayerDatasetProperty(id, "playCount", playCount);
            },

            startPlayer: function(id) {
                if (this.player.dataset.currentId != id) {
                    if ("volume" in this.music[id])
                        this.player.volume = this.music[id].volume;
                    this.player.dataset.currentId = id;
                    // Don't do this in iOS until the first activation
                    if (this.player.dataset.activated) {
                        this.player.src = this.music[id].src;
                        this.player.load();
                    }
                }
                return this.playPlayer(id);
            },

            playPlayer: function(id) {
                if (this.player.dataset.currentId == id
                    // For iOS autoplay requirements. This check is not
                    // necessary for Chrome/Firefox, but do it anyway
                    // for parity.
                    && (allowAutoplay || this.player.dataset.activated)
                )
                    return this.player.play();
            },

            pausePlayer: function(id) {
                if (this.player.dataset.currentId == id)
                    this.player.pause();
            },

            resetMusicState: function(id) {
                let context = 0;

                if (id && this.music[id]) {
                    this.pausePlayer(id);
                    this.setPlayerPlayCount(id, 0);
                    this.setPlayerCurrentTime(id, 0);
                    this.setPlayerLoop(id, false);
                    if (this.music[id].context)
                        context = this.music[id].context;
                }

                wasmTable.get(wasmMusicStopped)(context);
            },

            ////////////////////////////////////////////////////////////
            // Data Management
            ////////////////////////////////////////////////////////////

            createBlob: function(buf) {
                const type = this.getTypeFromMagic(buf);
                const blob = new Blob([buf], { type: type ? type : "octet/stream" });
                const url = URL.createObjectURL(blob);

                // TODO: Match blob by ptr and size so we don't duplicate

                if (!(url in this.blob))
                    this.blob[url] = 0;
                this.blob[url]++;

                return url;
            },

            deleteBlob: function(url) {
                if (url in this.blob && --this.blob[url] <= 0) {
                    URL.revokeObjectURL(url);
                    delete this.blob[url];
                }
            },

            createMusic: function(url, context) {
                const id = this.getNewId();
                this.music[id] = {
                    src: url
                };
                if (context)
                    this.music[id].context = context;
                return id;
            },

            deleteMusic: function(id) {
                if (!(id in this.music))
                    return;
                this.resetMusicState(id);
                this.deleteBlob(this.music[id].src);
                delete this.music[id];
            },

            getNewId: function() {
                const min = 1;
                const max = 2147483647; // INT32_MAX

                // Guard against collisions
                let id;
                do
                {
                    id = Math.floor(Math.random() * (max - min + 1) + min);
                } while(id in this.music);
                return id;
            },

            canPlayType: function(type) {
                // Allow user to create shortcuts, i.e. just "mp3"
                const formats = {
                    mp3: 'audio/mpeg',
                    ogg: 'audio/ogg',
                    wav: 'audio/wav',
                    flac: 'audio/flac',
                    mp4: 'audio/mp4',
                    m4a: 'audio/mp4',
                    aif: 'audio/x-aiff',
                    webm: 'audio/webm',
                    adts: 'audio/aac',
                    mkv: 'video/x-matroska',
                    mka: 'audio/x-matroska'
                };

                return !!this.player.canPlayType(formats[type] || type);
            },

            canPlayFile: function(file) {
                const type = file.split('.').pop();
                if (type)
                    return this.canPlayType(type);
                else // Fail without Exception
                    return false;
            },

            getTypeFromMagic: function(buf) {
                let result = null;

                const targets = [
                    { type: "audio/ogg", magic: [0x4f, 0x67, 0x67, 0x53] },  // OggS
                    { type: "audio/flac", magic: [0x66, 0x4c, 0x61, 0x43] }, // fLaC
                    //{ type: "audio/midi", magic: [0x4d, 0x54, 0x68, 0x64] }, // MThd
                    { type: "audio/mpeg", magic: [0x49, 0x44, 0x33] },        // ID3
                    { type: "audio/wav", magic: [0x52, 0x49, 0x46, 0x46] },
                    { type: "audio/mp4", offset: 4, magic: [0x66, 0x74, 0x79, 0x70, 0x69, 0x73, 0x6F, 0x6D] },
                    { type: "audio/mp4", offset: 4, magic: [0x66, 0x74, 0x79, 0x70, 0x4D, 0x34, 0x41, 0x20] },
                    { type: "audio/x-aiff", magic: [0x46, 0x4F, 0x52, 0x4D] },
                    { type: "audio/webm", magic: [0x1A, 0x45, 0xDF, 0xA3] }
                ];
                
                targets.some((target) => {
                    const targetMagic = target.magic;
                    const magicLength = targetMagic.length;
                    const offset = target.offset || 0;
                    const magic = buf.slice(offset, offset + magicLength);

                    let matching = true;
                    for (let i = 0; i < magicLength; i++) {
                        if (magic[i] !== targetMagic[i]) {
                            matching = false;
                            break;
                        }
                    }

                    if (matching) {
                        result = target.type;
                        return true;
                    }
                });

                // MP3 special case
                if (!result) {
                    const magic = buf.slice(0, 2);
                    if (magic[0] === 0xFF && (magic[1] & 0xFE) === 0xFA)
                        result = "audio/mpeg";
                }

                return result;
            },

            canPlayMagic: function(buf) {
                return this.canPlayType(this.getTypeFromMagic(buf));
            },

            ////////////////////////////////////////////////////////////
            // Events
            ////////////////////////////////////////////////////////////

            musicFinished: function(e) {
                const audio = e.target;
                const id = audio.dataset.currentId;

                if (!(audio instanceof HTMLMediaElement))
                    return;

                // if playCount == -1, then audio.loop is true and the
                // "ended" event is not fired (i.e., we never reach this function.)

                const playCount = audio.dataset.playCount - 1;
                Module["SDL2Mixer"].setPlayerPlayCount(id, playCount);

                if (playCount > 0) {
                    audio.currentTime = 0;
                    audio.play();
                } else
                    Module["SDL2Mixer"].resetMusicState(id);
            },

            musicError: function(e) {
                const audio = e.target;

                if (!(audio instanceof HTMLMediaElement))
                    return;

                err("Error " + audio.error.code + "; details: " + audio.error.message);

                Module["SDL2Mixer"].resetMusicState(audio.dataset.currentId);
            },

            musicInterrupted: function(e) {
                Module["SDL2Mixer"].resetMusicState(e.target.dataset.currentId);
            }
        };

        Module["SDL2Mixer"].player.addEventListener("ended", Module["SDL2Mixer"].musicFinished, false);
        Module["SDL2Mixer"].player.addEventListener("error", Module["SDL2Mixer"].musicError, false);
        Module["SDL2Mixer"].player.addEventListener("abort", Module["SDL2Mixer"].musicInterrupted, false);
        // Can browser recover from these states? If not, consider enabling these
        // as well as the corresponding removeEventListeners in deleteMusic().
        //Module["SDL2Mixer"].player.addEventListener("stalled", Module["SDL2Mixer"].musicInterrupted, false);
        //Module["SDL2Mixer"].player.addEventListener("suspend", Module["SDL2Mixer"].musicInterrupted, false);

        // Satisfy iOS input requirement for autoplay.
        // Based on https://github.com/emscripten-core/emscripten/pull/10843
        ["keydown","mousedown","touchstart"].forEach(function(event) {
            [document, document.getElementById("canvas")].forEach(function (element) {
                if (element)
                    element.addEventListener(event, function () {
                        if (Module["SDL2Mixer"] 
                            && Module["SDL2Mixer"].player 
                            && !Module["SDL2Mixer"].player.dataset.activated
                        ) {
                            if (Module["SDL2Mixer"].player.dataset.currentId) {
                                const id = parseInt(Module["SDL2Mixer"].player.dataset.currentId);
                                if (Module["SDL2Mixer"].music[id]) {
                                    Module["SDL2Mixer"].player.src = Module["SDL2Mixer"].music[id].src;
                                    Module["SDL2Mixer"].player.load();
                                }
                            }
                            Module["SDL2Mixer"].player.play();
                            Module["SDL2Mixer"].player.dataset.activated = true;
                        }
                    }, { once: true });
            });
        });
    }), html5_handle_music_stopped, SDL_MIXER_HTML5_ALLOW_AUTOPLAY);

    return 0;
}

static void *MusicHTML5_CreateFromRW(SDL_RWops *src, int freesrc)
{
    int id = -1;
    int size = src->size(src);
    MusicHTML5 *music = (MusicHTML5 *)SDL_calloc(1, sizeof *music);

    if (music == NULL) {
        Mix_SetError("Out of memory");
        return NULL;
    }

    SDL_bool force = SDL_MIXER_HTML5_DISABLE_TYPE_CHECK;

    if (src->type == SDL_RWOPS_STDFILE)
    {
        // This violates "private" membership, but this lets us avoid
        // copying the entire file into a new buffer. Instead, we query
        // FS for the file's location in MEMFS.
        FILE *f = src->hidden.stdio.fp;
        int fd = fileno(f);

        if (f) {
            id = EM_ASM_INT({
                const fd = $0;
                const context = $1;
                const force = $2;

                const stream = SYSCALLS.getStreamFromFD(fd);

                if (!stream || !stream.node || !stream.node.contents)
                    return -1;

                const buf = stream.node.contents;

                const canPlay = force
                    || Module["SDL2Mixer"].canPlayFile(stream.path)
                    || Module["SDL2Mixer"].canPlayMagic(buf);

                if (!canPlay)
                    return -1;

                const url = Module["SDL2Mixer"].createBlob(buf);
                const id = Module["SDL2Mixer"].createMusic(url, context);

                return id;
            }, fd, music, force);
        }
    }
    else if (src->type == SDL_RWOPS_MEMORY || src->type == SDL_RWOPS_MEMORY_RO)
    {
        // This violates "private" membership, but it works because
        // the entire file is loaded in this pointer.
        void *buf = src->hidden.mem.base;

        if (buf && size > 0)
        {
            id = EM_ASM_INT({
                const ptr = $0;
                const size = $1;
                const context = $2;
                const force = $3;

                const buf = new Uint8Array(Module.HEAPU8.buffer, ptr, size);

                const canPlay = force || Module["SDL2Mixer"].canPlayMagic(buf);

                if (!canPlay)
                    return -1;

                const url = Module["SDL2Mixer"].createBlob(buf);
                const id = Module["SDL2Mixer"].createMusic(url, context);

                return id;
            }, buf, size, music, force);
        }
    } 
    else
    {
        Mix_SetError("Unsupported RWops type: %d", src->type);
        if (freesrc)
            SDL_RWclose(src);
        return NULL;
    }

    if (id == -1)
    {
        SDL_free(music);
        return NULL;
    }

    /* Fill the music structure */
    music->id = id;
    music->src = src;
    music->freesrc = freesrc;
    music->playing = SDL_TRUE;

    /* We're done */
    return music;
}

/* Load a music stream from the given file */
static void *MusicHTML5_CreateFromFile(const char *file)
{
    MusicHTML5 *music = (MusicHTML5 *)SDL_calloc(1, sizeof *music);;
    int id = -1;
    SDL_bool force = SDL_MIXER_HTML5_DISABLE_TYPE_CHECK;

    if (music == NULL) {
        Mix_SetError("Out of memory");
        return NULL;
    }

    id = EM_ASM_INT({
        const file = UTF8ToString($0);
        const context = $1;
        const force = $2;

        let url;
        try {
            // Is path in FS?
            const buf = FS.readFile(file);
            url = Module["SDL2Mixer"].createBlob(buf);

            const canPlay = force 
                || Module["SDL2Mixer"].canPlayFile(file)
                || Module["SDL2Mixer"].canPlayMagic(buf);

            if (!canPlay) {
                Module["SDL2Mixer"].deleteBlob(url);
                return -1;
            }
        } catch(e) {
            // Fail silently, presume file not in FS.
            // Assume it's a relative or absolute URL
            url = file;

            // Check audio capability by file extension
            if (!force && !Module["SDL2Mixer"].canPlayFile(url))
                return -1;
        }

        const id = Module["SDL2Mixer"].createMusic(url, context);
        return id;
    }, file, music, force);

    if (id == -1) {
        SDL_free(music);
        return NULL;
    }

    /* Fill the music structure */
    music->id = id;
    music->freesrc = SDL_FALSE;
    music->playing = SDL_TRUE;

    /* We're done */
    return music;
}

/* Set the volume for a given music stream */
static void MusicHTML5_SetVolume(void *context, int volume)
{
    MusicHTML5 *music = (MusicHTML5 *)context;
    float normalized_volume = ((float)volume) / MIX_MAX_VOLUME;

    EM_ASM({
        const id = $0;
        const volume = Math.min(Math.max(0, $1), 1);
        Module["SDL2Mixer"].setPlayerVolume(id, volume);
    }, music->id, normalized_volume);
}

static void MusicHTML5_Stop(void *context);

/* Start playback of a given music stream */
static int MusicHTML5_Play(void *context, int play_count)
{
    MusicHTML5 *music = (MusicHTML5 *)context;

    if (play_count <= 0 && play_count != -1)
    {
        // do not play but do not throw errors either.
        // TODO: What is SDL Mixer's state when passing play_count <= 0?
        MusicHTML5_Stop(context);
        return 0;
    }
    int status = EM_ASM_INT({
        try {
            const id = $0;
            const playCount = $1;

            // TODO: Asyncify Promise
            const played = Module["SDL2Mixer"].startPlayer(id);

            // Older browsers do not return a Promise
            if (played)
                played.catch((e) => err(e));

            // Retain play_count for handling in musicFinished()
            Module["SDL2Mixer"].setPlayerPlayCount(id, playCount);

            // If play_count == -1, we are looping
            Module["SDL2Mixer"].setPlayerLoop(id, (playCount == -1));
        } catch (e) {
            err(e);
            return -1;
        }
        return 0;
    }, music->id, play_count);

    if (status < 0)
        Mix_SetError("Emscripten HTML5 error, see developer console.");
    
    return status;
}

/* Return non-zero if a stream is currently playing */
static SDL_bool MusicHTML5_IsPlaying(void *context)
{
    MusicHTML5 *music = (MusicHTML5 *)context;
    
    if (!music) {
        // Call "finished" handler in devappd/html5_mixer
        html5_handle_music_stopped(context);
        return SDL_FALSE;
    }

    // We track a music->playing variable to play nice with music_mixer()'s
    // IsPlaying() check on every frame. E.g., the check will run when
    // <audio> is buffering and the music is technically not "playing".
    // Ergo, the HookMusicFinished() callback is called immediately when
    // the <audio> has not even begun playback.
    //
    // To resolve this, we rely on JavaScript callbacks to reset the
    // music->playing status on end, on error, etc.

    int safeStatus = EM_ASM_INT({
        const id = $0;
        const safeStatus =
            Module["SDL2Mixer"].player
            && Module["SDL2Mixer"].player.dataset.currentId == id
            && !Module["SDL2Mixer"].player.ended
            // SDL Mixer considers "paused" music as "playing"
            //&& !Module["SDL2Mixer"].player.paused
            // These conditions interfere with the "playing" check
            //&&  Module["SDL2Mixer"].player.readyState > 2;
            //&&  Module["SDL2Mixer"].player.currentTime > 0
            ;

        if (!safeStatus)
            // Reset JS state and falsify music->playing
            Module["SDL2Mixer"].resetMusicState(id);
    }, music->id);

    return music->playing;
}

/* Jump (seek) to a given position (time is in seconds) */
static int MusicHTML5_Seek(void *context, double time)
{
    MusicHTML5 *music = (MusicHTML5 *)context;

    EM_ASM({
        const id = $0;
        const time = $1;
        Module["SDL2Mixer"].setPlayerCurrentTime(id, time);
    }, music->id, time);

    return 0;
}

/* Pause playback of a given music stream */
static void MusicHTML5_Pause(void *context)
{
    MusicHTML5 *music = (MusicHTML5 *)context;

    EM_ASM({
        const id = $0;
        Module["SDL2Mixer"].pausePlayer(id);
    }, music->id);
}

/* Resume playback of a given music stream */
static void MusicHTML5_Resume(void *context)
{
    MusicHTML5 *music = (MusicHTML5 *)context;

    EM_ASM({
        const id = $0;
        Module["SDL2Mixer"].playPlayer(id);
    }, music->id);
}

/* Stop playback of a stream previously started with MusicHTML5_Start() */
static void MusicHTML5_Stop(void *context)
{
    MusicHTML5 *music = (MusicHTML5 *)context;

    EM_ASM({
        const id = $0;
        Module["SDL2Mixer"].resetMusicState(id);
    }, music->id);
}

/* Close the given music stream */
static void MusicHTML5_Delete(void *context)
{
    MusicHTML5 *music = (MusicHTML5 *)context;

    if (html5_opened()) {
        EM_ASM({
            const id = $0;
            if (id in Module["SDL2Mixer"].music)
                Module["SDL2Mixer"].deleteMusic(id);
        }, music->id);
    }

    if (music->freesrc && music->src)
        SDL_RWclose(music->src);

    SDL_free(music);
}

static void MusicHTML5_Close(void)
{
    if (!html5_opened())
        return;

    EM_ASM({
        for(const prop in Module["SDL2Mixer"].music) {
            Module["SDL2Mixer"].deleteMusic(prop);
        }

        Module["SDL2Mixer"].player.pause();
        Module["SDL2Mixer"].player.removeAttribute("src");
        Module["SDL2Mixer"].player.load();
        Module["SDL2Mixer"].player.remove();

        Module["SDL2Mixer"].player.removeEventListener("ended", Module["SDL2Mixer"].musicFinished, false);
        Module["SDL2Mixer"].player.removeEventListener("error", Module["SDL2Mixer"].musicError, false);
        Module["SDL2Mixer"].player.removeEventListener("abort", Module["SDL2Mixer"].musicInterrupted, false);
        //Module["SDL2Mixer"].player.removeEventListener("stalled", Module["SDL2Mixer"].musicInterrupted, false);
        //Module["SDL2Mixer"].player.removeEventListener("suspend", Module["SDL2Mixer"].musicInterrupted, false);

        delete Module["SDL2Mixer"];
    });
}

Mix_MusicInterface Mix_MusicInterface_HTML5 =
{
    "HTML5",
    MIX_MUSIC_HTML5,
    MUS_HTML5,
    SDL_FALSE,
    SDL_FALSE,

    NULL,   /* Load */
    MusicHTML5_Open,
    MusicHTML5_CreateFromRW,
    MusicHTML5_CreateFromFile,
    MusicHTML5_SetVolume,
    MusicHTML5_Play,
    MusicHTML5_IsPlaying,
    NULL,   /* GetAudio */
    MusicHTML5_Seek,
    MusicHTML5_Pause,
    MusicHTML5_Resume,
    MusicHTML5_Stop,
    MusicHTML5_Delete,
    MusicHTML5_Close,
    NULL,   /* Unload */
};

#endif /* MUSIC_HTML5 */

/* vi: set ts=4 sw=4 expandtab: */
