
export ANDROID_SDK=/home/terop/Android/Sdk
# Assuming ANDROID_SDK is set to your SDK path
AAPT=$ANDROID_SDK/build-tools/35.0.0/aapt
(cd android_cmdline;$AAPT package -f -M AndroidManifest.xml -S res -I $ANDROID_SDK/platforms/android-35/android.jar -F ../gameapi_cmdline.apk)

# Add your native library
(cd android_cmdline ; zip -u ../gameapi_cmdline.apk lib/arm64-v8a/android_aout)


keytool -genkey -v -keystore debug.keystore -alias androiddebugkey -keyalg RSA -keysize 2048 -validity 10000
# Sign the APK (you'll need a keystore)
(cd android_cmdline; jarsigner -keystore ../debug.keystore ../gameapi_cmdline.apk androiddebugkey)
