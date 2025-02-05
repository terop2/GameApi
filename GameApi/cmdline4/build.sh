export ARCH1=armv7a-eabi
export ARCH2=aarch64
export ARCH3=x86
export ARCH4=x86_64

array1=("$ARCH1" "$ARCH2" "$ARCH3" "$ARCH4")
for i in "${!array1[@]}"; do
    export ARCH="${array1[$i]}"
    export ANDROID_NDK=/home/terop/Android/Sdk/ndk/28.0.12674087/
if [ "$ARCH" = "x86_64" ]; then
    export SDL=/home/terop/cvs/GameApi/GameApi/androiddeps/SDLBuild4
    export ARCH2=x86_64
fi
if [ "$ARCH" = "x86" ]; then
    export SDL=/home/terop/cvs/GameApi/GameApi/androiddeps/SDLBuild3
    export ARCH2=x86
fi

if [ "$ARCH" = "armv7a-eabi" ]; then
    export SDL=/home/terop/cvs/GameApi/GameApi/androiddeps/SDLBuild2
    export ARCH2=armeabi-v7a
fi
if [ "$ARCH" = "aarch64" ]; then
    export SDL=/home/terop/cvs/GameApi/GameApi/androiddeps/SDLBuild
    export ARCH2=arm64-v8a
fi

export FREETYPE=/home/terop/cvs/GameApi/GameApi/cmdline4/android_cmdline/lib/$ARCH2/
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
    ../cmdline/test.cpp \
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
