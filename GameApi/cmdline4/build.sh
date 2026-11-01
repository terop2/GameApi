if [[ $1 == "" || $2 == "" ]]; then
    echo "Usage: ./build.sh package_name package_id"
    exit
fi

hash=$(echo "$2" | sha1sum | awk '{print $1}')

echo "$hash"

cat > "jni_register.cpp" <<EOF
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

AAssetManager *g_assetManager=nullptr;

extern "C"
JNIEXPORT void JNICALL nativeInit(
									 JNIEnv *env,
									 jobject thiz,
									 jobject mgr_object)
{
  g_assetManager = AAssetManager_fromJava(env,mgr_object);
}


static JNINativeMethod methods[] = {
  {
    "nativeInit",
    "(Landroid/content/res/AssetManager;)V",
    (void*)nativeInit
  }
};

extern "C"
JNIEXPORT void JNICALL
Java_com_meshpage_gameapi_1cmdline${hash}_MainActivity_registerNativeMethods(
    JNIEnv* env,
    jclass,
    jclass cls)
{
    env->RegisterNatives(cls, methods, sizeof(methods)/sizeof(methods[0]));
}
EOF

export ARCH1=armv7a-eabi
export ARCH2=aarch64
export ARCH3=x86
export ARCH4=x86_64

export GAMEAPI=/home/terop/cvs/GameApi_newer_features/GameApi

array1=("$ARCH1" "$ARCH2" "$ARCH3" "$ARCH4")
for i in "${!array1[@]}"; do
    export ARCH="${array1[$i]}"
    export ANDROID_NDK=/home/terop/Android/Sdk/ndk/28.0.12674087/
if [ "$ARCH" = "x86_64" ]; then
    export SDL=$GAMEAPI/androiddeps/SDLBuild4
    export ARCH2=x86_64
fi
if [ "$ARCH" = "x86" ]; then
    export SDL=$GAMEAPI/androiddeps/SDLBuild3
    export ARCH2=x86
fi

if [ "$ARCH" = "armv7a-eabi" ]; then
    export SDL=$GAMEAPI/androiddeps/SDLBuild2
    export ARCH2=armeabi-v7a
fi
if [ "$ARCH" = "aarch64" ]; then
    export SDL=$GAMEAPI/androiddeps/SDLBuild
    export ARCH2=arm64-v8a
fi

export FREETYPE=$GAMEAPI/cmdline4/android_cmdline/lib/$ARCH2/
export SDLLIBS=$SDL

export COMPILE_PATH=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin

if [ "$ARCH" = "x86" ]; then
   export C_COMPILER=$COMPILE_PATH/i686-linux-android30-clang
   export CPP_COMPILER=$COMPILE_PATH/i686-linux-android30-clang++
fi
if [ "$ARCH" = "x86_64" ]; then
   export C_COMPILER=$COMPILE_PATH/x86_64-linux-android30-clang
   export CPP_COMPILER=$COMPILE_PATH/x86_64-linux-android30-clang++
fi

if [ "$ARCH" = "armv7a-eabi" ]; then
   export C_COMPILER=$COMPILE_PATH/armv7a-linux-androideabi30-clang
   export CPP_COMPILER=$COMPILE_PATH/armv7a-linux-androideabi30-clang++
fi
if [ "$ARCH" = "aarch64" ]; then
   export C_COMPILER=$COMPILE_PATH/aarch64-linux-android21-clang
   export CPP_COMPILER=$COMPILE_PATH/aarch64-linux-android21-clang++
fi

$C_COMPILER -c $ANDROID_NDK/sources/android/native_app_glue/android_native_app_glue.c \
    -o android_native_app_glue.o -fPIC
   

# Compile native_app_glue
sed "s/GameApiCmdLine/$1/g" ../cmdline/test.cpp >test0.cpp
cat test0.cpp jni_register.cpp >test.cpp

# Main compilation
$CPP_COMPILER \
    -I$ANDROID_NDK/sources/android/native_app_glue \
    -I$SDL/include \
    -I$SDL/include/SDL2 \
    -I$SDL/include-config-release \
    -I$SDL/include-config-release/SDL2 \
    -I.. \
    -DANDROID \
    -D_FILE_OFFSET_BITS=64 \
    test.cpp \
    android_native_app_glue.o \
    -L$SDLLIBS \
    -L$FREETYPE \
    -L.. \
    -L. \
    -lGameApi_android-$ARCH \
    -lfreetype \
    -lSDL2 \
    -lSDL2main \
    -lGLESv1_CM \
    -lGLESv2 \
    -lEGL \
    -landroid \
    -llog \
    -lc \
    -pthread \
    -Wl,--allow-shlib-undefined \
    -fPIC \
    -shared -Wl,--no-undefined \
    -static-libstdc++ \
    -o android_aout-$ARCH

done
