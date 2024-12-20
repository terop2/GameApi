# ======= Main Build Configuration =======
SDL_ROOT := /home/terop/cvs/GameApi/GameApi/androiddeps/SDL
ANDROID_PROJECT := $(SDL_ROOT)/android-project

# Copy necessary files to SDL Android project
setup:
	# Create jni directory if it doesn't exist
	mkdir -p $(ANDROID_PROJECT)/app/jni
	# Copy main.cpp to the jni directory
	cp main.cpp $(ANDROID_PROJECT)/app/jni/src/

# ======= Generate Android.mk =======
define ANDROID_MK_CONTENT
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_ROOT)/include

LOCAL_SRC_FILES := src/main.cpp

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -landroid

include $(BUILD_SHARED_LIBRARY)
endef
export ANDROID_MK_CONTENT

# ======= Generate Application.mk =======
define APPLICATION_MK_CONTENT
APP_ABI := armeabi-v7a
APP_STL := c++_shared
APP_PLATFORM := android-21
endef
export APPLICATION_MK_CONTENT

all: setup
	# Create the Android.mk and Application.mk files
	@echo "$$ANDROID_MK_CONTENT" > $(ANDROID_PROJECT)/app/jni/Android.mk
	@echo "$$APPLICATION_MK_CONTENT" > $(ANDROID_PROJECT)/app/jni/Application.mk
	# Build using Gradle
	cd $(ANDROID_PROJECT) && ./gradlew assembleDebug

clean:
	cd $(ANDROID_PROJECT) && ./gradlew clean

.PHONY: all clean setup
