
export ARCH=armv7a-eabi
#export ARCH=aarch64

export ANDROID_SDK=/home/terop/Android/Sdk
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
export SDL=/home/terop/cvs/GameApi/GameApi/androiddeps/SDLBuild
export NDK=/home/terop/Android/Sdk/ndk/28.0.12674087

export AARCH_DIR_EABI=armv7a-eabi
export AARCH_DIR2_EABI=armeabi-v7a
export ANDROID_CURL_LIB_EABI=/home/terop/cvs/GameApi/GameApi/androiddeps/vcpkg/installed/arm-android/lib
export AARCH_DIR_ARM64=aarch64
export AARCH_DIR2_ARM64=arm64-v8a
export ANDROID_CURL_LIB_ARM64=/home/terop/cvs/GameApi/GameApi/androiddeps/vcpkg/installed/arm64-android/lib
export AARCH_DIR_X86=x86
export AARCH_DIR2_X86=x86
export ANDROID_CURL_LIB_X86=/home/terop/cvs/GameApi/GameApi/androiddeps/vcpkg/installed/x86-android/lib
export AARCH_DIR_X86_64=x86_64
export AARCH_DIR2_X86_64=x86_64
export ANDROID_CURL_LIB_X86_64=/home/terop/cvs/GameApi/GameApi/androiddeps/vcpkg/installed/x64-android/lib


# Assuming ANDROID_SDK is set to your SDK path
AAPT=$ANDROID_SDK/build-tools/35.0.0/aapt
JAVAC="javac"
ANDROID_JAR="$ANDROID_SDK/platforms/android-26/android.jar"
(cd android_cmdline;$AAPT package -f -m -J src -M AndroidManifest.xml -S res -I $ANDROID_SDK/platforms/android-35/android.jar -F ../gameapi_cmdline.apk)

if [ ! -f "$ANDROID_JAR" ]; then
    echo "Error: $ANDROID_JAR not found!"
    exit 1
fi

array1=("$AARCH_DIR_EABI" "$AARCH_DIR_ARM64" "$AARCH_DIR_X86" "$AARCH_DIR_X86_64")
array3=("$AARCH_DIR2_EABI" "$AARCH_DIR2_ARM64" "$AARCH_DIR2_X86" "$AARCH_DIR2_X86_64")
array2=("$ANDROID_CURL_LIB_EABI" "$ANDROID_CURL_LIB_ARM64" "$ANDROID_CURL_LIB_X86" "$ANDROID_CURL_LIB_X86_64")

for i in "${!array1[@]}"; do
    export ARCH="${array1[$i]}"
    export AARCH_DIR="${array3[$i]}"
    echo "Copying from $ARCH to $AARCH_DIR"
    cp android_aout-$ARCH android_cmdline/lib/$AARCH_DIR/libmain.so
    cp ../libGameApi_android-$ARCH.so android_cmdline/lib/$AARCH_DIR/
done
#cp $SDL/libSDL2.so android_cmdline/lib/arm64-v8a/libSDL2.so
#cp ../libGameApi_android.so android_cmdline/lib/arm64-v8a/libGameApi_android.so
#cp ../androiddeps/freetype-2.13.3/build/.libs/libfreetype.so android_cmdline/lib/arm64-v8a/libfreetype.so
#cp $NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so android_cmdline/lib/arm64-v8a/libc++_shared.so
# Add your native library

(cd android_cmdline; mkdir -p obj; $JAVAC -classpath "$ANDROID_JAR" -d obj \
					  src/org/libsdl/app/*.java)


(cd android_cmdline; mkdir -p obj; $JAVAC -classpath "obj:$ANDROID_JAR" -d obj src/com/meshpage/gameapi_cmdline/MainActivity.java \
					  src/com/meshpage/gameapi_cmdline/R.java \
    )

(cd android_cmdline; $ANDROID_SDK/build-tools/35.0.0/d8 obj/org/libsdl/app/*.class obj/com/meshpage/gameapi_cmdline/*.class --lib $ANDROID_JAR --output .)

(cd android_cmdline ; zip -u ../gameapi_cmdline.apk src/com/meshpage/gameapi_cmdline/MainActivity.java)
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk classes.dex)
cp script.txt android_cmdline/assets/
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk assets/script.txt)


for i in "${!array1[@]}"; do
    export ARCH="${array1[$i]}"
    export AARCH_DIR="${array3[$i]}"
    (cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/$AARCH_DIR/libSDL2.so)
    (cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/$AARCH_DIR/libmain.so)
    (cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/$AARCH_DIR/libGameApi_android-$ARCH.so)
    (cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/$AARCH_DIR/libfreetype.so)
    (cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/$AARCH_DIR/libc++_shared.so)
    (cd android_cmdline ; zip -u ../gameapi_cmdline.apk store/ground.png)
done
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk res/values/strings.xml)
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk res/mipmap/ic_launcher.png)

$ANDROID_SDK/build-tools/35.0.0/zipalign -f 4 gameapi_cmdline.apk gameapi_cmdline_aligned.apk
mv gameapi_cmdline_aligned.apk gameapi_cmdline.apk

keytool -genkey -v -keystore debug.keystore -alias androiddebugkey -keyalg RSA -keysize 2048 -validity 10000
# Sign the APK (you'll need a keystore)
$ANDROID_SDK/build-tools/35.0.0/apksigner sign --ks debug.keystore gameapi_cmdline.apk

