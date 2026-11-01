package com.meshpage.gameapi_cmdline580a09be289b4612d967ec8bf33daf864ff17738;

import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {
    static {
        try {
	    System.loadLibrary("SDL2");
	    System.loadLibrary("GameApi_android");
	    System.loadLibrary("freetype");
	    System.loadLibrary("c++_shared");
        } catch (Throwable t) {
        }
    }
}
