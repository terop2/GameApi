# ======= Main Build Configuration =======
# Paths - update these to match your setup
ANDROID_SDK_ROOT := $(HOME)/Android/Sdk
ANDROID_NDK_ROOT := $(ANDROID_SDK_ROOT)/ndk/28.0.12674087
SDL_ROOT := $(HOME)/GameApi/GameApi/androiddeps/SDL2

# App settings
APP_NAME := MeshPageCmdLine
PACKAGE_NAME := com.meshpage.gameapi
ANDROID_PLATFORM := android-33

# Tools from SDK
AAPT := $(ANDROID_SDK_ROOT)/build-tools/33.0.0/aapt
DX := $(ANDROID_SDK_ROOT)/build-tools/33.0.0/dx
ZIPALIGN := $(ANDROID_SDK_ROOT)/build-tools/33.0.0/zipalign

# Output directories and files
BUILD_DIR := build
APK_UNSIGNED := $(BUILD_DIR)/$(APP_NAME)-unsigned.apk
APK_SIGNED := $(BUILD_DIR)/$(APP_NAME)-signed.apk
APK_FINAL := $(BUILD_DIR)/$(APP_NAME)-final.apk
KEYSTORE := my-release-key.keystore
NATIVE_LIB := $(BUILD_DIR)/lib/armeabi-v7a/libmain.so
DEX_FILE := $(BUILD_DIR)/classes.dex

# ======= Auto-generated Directories =======
$(shell mkdir -p $(BUILD_DIR)/lib/armeabi-v7a)
$(shell mkdir -p $(BUILD_DIR)/obj)
$(shell mkdir -p $(BUILD_DIR)/jni)

# ======= Generate Android.mk =======
define ANDROID_MK_CONTENT
LOCAL_PATH := $$(call my-dir)

include $$(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_SRC_FILES := main.cpp
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog
LOCAL_C_INCLUDES := $(SDL_ROOT)/include
include $$(BUILD_SHARED_LIBRARY)

$$(call import-module,SDL)
endef
export ANDROID_MK_CONTENT

# ======= Generate Application.mk =======
define APPLICATION_MK_CONTENT
APP_ABI := armeabi-v7a
APP_STL := c++_shared
APP_PLATFORM := $(ANDROID_PLATFORM)
endef
export APPLICATION_MK_CONTENT

# ======= Generate AndroidManifest.xml =======
define MANIFEST_CONTENT
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="$(PACKAGE_NAME)"
    android:versionCode="1"
    android:versionName="1.0">
    
    <application
        android:label="@string/app_name"
        android:hasCode="true">
        <activity android:name=".MainActivity"
            android:label="@string/app_name"
            android:configChanges="keyboard|keyboardHidden|orientation"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
            <meta-data android:name="android.app.lib_name" android:value="main" />
        </activity>
    </application>
    
    <uses-sdk android:minSdkVersion="21" android:targetSdkVersion="33" />
    <uses-feature android:glEsVersion="0x00020000" />
</manifest>
endef
export MANIFEST_CONTENT

# ======= Generate MainActivity.java =======
define MAIN_ACTIVITY_CONTENT
package $(PACKAGE_NAME);

import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "main"
        };
    }
}
endef
export MAIN_ACTIVITY_CONTENT

# ======= Main Build Targets =======
.PHONY: all clean setup

all: setup $(APK_FINAL)

setup:
	# Create build files
	@echo "$$ANDROID_MK_CONTENT" > $(BUILD_DIR)/jni/Android.mk
	@echo "$$APPLICATION_MK_CONTENT" > $(BUILD_DIR)/jni/Application.mk
	@echo "$$MANIFEST_CONTENT" > $(BUILD_DIR)/AndroidManifest.xml
	@mkdir -p $(BUILD_DIR)/src/$(subst .,/,$(PACKAGE_NAME))
	@echo "$$MAIN_ACTIVITY_CONTENT" > $(BUILD_DIR)/src/$(subst .,/,$(PACKAGE_NAME))/MainActivity.java

$(NATIVE_LIB): setup
	cd $(BUILD_DIR) && $(ANDROID_NDK_ROOT)/ndk-build \
		NDK_PROJECT_PATH=. \
		APP_BUILD_SCRIPT=jni/Android.mk \
		NDK_APPLICATION_MK=jni/Application.mk \
		APP_PLATFORM=$(ANDROID_PLATFORM) \
		APP_ABI=armeabi-v7a \
		SDL_PATH=$(SDL_ROOT)

$(DEX_FILE): setup
	$(JAVAC) -d $(BUILD_DIR)/obj \
		-classpath $(ANDROID_SDK_ROOT)/platforms/$(ANDROID_PLATFORM)/android.jar:$(SDL_ROOT)/android-project/app/src/main/java \
		$(BUILD_DIR)/src/$(subst .,/,$(PACKAGE_NAME))/MainActivity.java
	$(DX) --dex --output=$(DEX_FILE) $(BUILD_DIR)/obj

$(KEYSTORE):
	keytool -genkey -v -keystore $(KEYSTORE) \
		-alias androidkey -keyalg RSA -keysize 2048 -validity 10000 \
		-storepass android -keypass android \
		-dname "CN=Android Debug,O=Android,C=US"

$(APK_UNSIGNED): $(NATIVE_LIB) $(DEX_FILE)
	$(AAPT) package -f \
		-M $(BUILD_DIR)/AndroidManifest.xml \
		-I $(ANDROID_SDK_ROOT)/platforms/$(ANDROID_PLATFORM)/android.jar \
		-F $(APK_UNSIGNED)
	cd $(BUILD_DIR) && $(AAPT) add ../$(APK_UNSIGNED) classes.dex
	cd $(BUILD_DIR) && $(AAPT) add ../$(APK_UNSIGNED) lib/armeabi-v7a/libmain.so
	$(AAPT) add $(APK_UNSIGNED) $(SDL_ROOT)/android-project/app/build/intermediates/merged_native_libs/debug/out/lib/armeabi-v7a/libSDL2.so

$(APK_SIGNED): $(APK_UNSIGNED) $(KEYSTORE)
	jarsigner -sigalg SHA1withRSA -digestalg SHA1 \
		-keystore $(KEYSTORE) -storepass android \
		-keypass android $(APK_UNSIGNED) androidkey \
		-signedjar $(APK_SIGNED)

$(APK_FINAL): $(APK_SIGNED)
	$(ZIPALIGN) -f 4 $(APK_SIGNED) $(APK_FINAL)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(KEYSTORE)
