
export ANDROID_SDK=/home/terop/Android/Sdk
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
export SDL=/home/terop/cvs/GameApi/GameApi/androiddeps/SDLBuild
export NDK=/home/terop/Android/Sdk/ndk/28.0.12674087
export ANDROID_CURL_LIB=/home/terop/cvs/GameApi/GameApi/androiddeps/vcpkg/installed/arm64-android/lib

# Assuming ANDROID_SDK is set to your SDK path
AAPT=$ANDROID_SDK/build-tools/35.0.0/aapt
JAVAC="javac"
ANDROID_JAR="$ANDROID_SDK/platforms/android-26/android.jar"
(cd android_cmdline;$AAPT package -f -m -J src -M AndroidManifest.xml -S res -I $ANDROID_SDK/platforms/android-35/android.jar -F ../gameapi_cmdline.apk)

if [ ! -f "$ANDROID_JAR" ]; then
    echo "Error: $ANDROID_JAR not found!"
    exit 1
fi

cp android_aout android_cmdline/lib/arm64-v8a/libmain.so
cp $SDL/libSDL2.so android_cmdline/lib/arm64-v8a/libSDL2.so
cp ../libGameApi_android.so android_cmdline/lib/arm64-v8a/libGameApi_android.so
cp ../androiddeps/freetype-2.13.3/build/.libs/libfreetype.so android_cmdline/lib/arm64-v8a/libfreetype.so
cp $NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so android_cmdline/lib/arm64-v8a/libc++_shared.so
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
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/arm64-v8a/libSDL2.so)
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/arm64-v8a/libmain.so)
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/arm64-v8a/libGameApi_android.so)
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/arm64-v8a/libfreetype.so)
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/arm64-v8a/libc++_shared.so)
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk res/values/strings.xml)
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk res/mipmap/ic_launcher.png)

$ANDROID_SDK/build-tools/35.0.0/zipalign -f 4 gameapi_cmdline.apk gameapi_cmdline_aligned.apk
mv gameapi_cmdline_aligned.apk gameapi_cmdline.apk

keytool -genkey -v -keystore debug.keystore -alias androiddebugkey -keyalg RSA -keysize 2048 -validity 10000
# Sign the APK (you'll need a keystore)
$ANDROID_SDK/build-tools/35.0.0/apksigner sign --ks debug.keystore gameapi_cmdline.apk

