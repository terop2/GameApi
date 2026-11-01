package com.meshpage.gameapi_cmdline;

import org.libsdl.app.SDLActivity;
import android.content.res.AssetManager;
import android.os.Bundle;

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
    private static native void registerNativeMethods(Class<?> cls);
    public native void nativeInit(AssetManager mgr);
    @Override
    protected void onCreate(Bundle savedInstanceState) {
	super.onCreate(savedInstanceState);
	registerNativeMethods(MainActivity.class);
	nativeInit(getAssets());
    }
}
