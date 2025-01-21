export ANDROID_NDK=/home/terop/Android/Sdk/ndk/28.0.12674087/
export SDL=/home/terop/cvs/GameApi/GameApi/androiddeps/SDL2
export FREETYPE=/home/terop/cvs/GameApi/GameApi/androiddeps/freetype-2.13.3/build/.libs
export SDLLIBS=/home/terop/cvs/GameApi/GameApi/androiddeps/SDL2_2/lib

# Compile native_app_glue
$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang \
    -c $ANDROID_NDK/sources/android/native_app_glue/android_native_app_glue.c \
    -o android_native_app_glue.o

# Main compilation
$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang++ \
    -I$ANDROID_NDK/sources/android/native_app_glue \
    -I$SDL/include \
    -I.. \
    -DANDROID \
    -D_FILE_OFFSET_BITS=64 \
    ../cmdline/test.cpp \
    android_native_app_glue.o \
    -L$SDLLIBS \
    -L$FREETYPE \
    -L.. \
    -L. \
    -lGameApi_android \
    -lfreetype \
    -lSDL2 \
    -lGLESv1_CM \
    -lGLESv2 \
    -lEGL \
    -landroid \
    -llog \
    -lc \
    -pthread \
    -Wl,--allow-shlib-undefined \
    -o android_aout

