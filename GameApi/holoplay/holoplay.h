#pragma once

#if _WIN32
	#define EXPORT_API __declspec(dllexport)
#else
	#define EXPORT_API 
#endif

#include <string>

#ifdef HP_LOAD_LIBRARY
	#if _WIN32
		#include <windows.h>
		#include <iostream>
		#include <stdio.h>
		#define IMPORT_API __cdecl	
	#else
		#define IMPORT_API
	#endif
    typedef void (IMPORT_API* hp_loadCalibration_type)();
    hp_loadCalibration_type hp_loadCalibration;
    typedef void (IMPORT_API* hp_loadLightfieldShaders_type)();
    hp_loadLightfieldShaders_type hp_loadLightfieldShaders;
    typedef void (IMPORT_API* hp_loadBlitShaders_type)();
    hp_loadBlitShaders_type hp_loadBlitShaders;
    typedef void (IMPORT_API* hp_loadCalibrationIntoShader_type)();
    hp_loadCalibrationIntoShader_type hp_loadCalibrationIntoShader;
    typedef void (IMPORT_API* hp_setupQuiltSettings_type)(unsigned int);
    hp_setupQuiltSettings_type hp_setupQuiltSettings;
    typedef void (IMPORT_API* hp_setupCustomQuiltSettings_type)(unsigned int, unsigned int, 
		unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    hp_setupCustomQuiltSettings_type hp_setupCustomQuiltSettings;
    typedef void (IMPORT_API* hp_setupQuiltTexture_type)();
    hp_setupQuiltTexture_type hp_setupQuiltTexture;
    typedef void (IMPORT_API* hp_copyViewToQuilt_type)(unsigned int);
    hp_copyViewToQuilt_type hp_copyViewToQuilt;
    typedef void (IMPORT_API* hp_drawLightfield_type)();
    hp_drawLightfield_type hp_drawLightfield;
    typedef void (IMPORT_API* hp_setupBuffers_type)();
    hp_setupBuffers_type hp_setupBuffers;
    typedef void (IMPORT_API* hp_initialize_type)();
    hp_initialize_type hp_initialize;
    typedef void (IMPORT_API* hp_release_type)();
    hp_release_type hp_release;
    typedef int (IMPORT_API* load_calibration_type)(char *);
    load_calibration_type load_calibration;
    typedef void (IMPORT_API* get_version_type)(char *);
    get_version_type get_version;

	#if _WIN32
	void hp_loadLibrary(char * path = (char*)"HoloPlayAPI") {
		auto dll = LoadLibraryA(path);
		// make func pointers
        hp_loadCalibration = (hp_loadCalibration_type) GetProcAddress(dll, "hp_loadCalibration");
        hp_loadLightfieldShaders = (hp_loadLightfieldShaders_type) GetProcAddress(dll, "hp_loadLightfieldShaders");
        hp_loadBlitShaders = (hp_loadBlitShaders_type) GetProcAddress(dll, "hp_loadBlitShaders");
        hp_loadCalibrationIntoShader = (hp_loadCalibrationIntoShader_type) GetProcAddress(dll, "hp_loadCalibrationIntoShader");
        hp_setupQuiltSettings = (hp_setupQuiltSettings_type) GetProcAddress(dll, "hp_setupQuiltSettings");
        hp_setupCustomQuiltSettings = (hp_setupCustomQuiltSettings_type) GetProcAddress(dll, "hp_setupCustomQuiltSettings");
        hp_setupQuiltTexture = (hp_setupQuiltTexture_type) GetProcAddress(dll, "hp_setupQuiltTexture");
        hp_copyViewToQuilt = (hp_copyViewToQuilt_type) GetProcAddress(dll, "hp_copyViewToQuilt");
        hp_drawLightfield = (hp_drawLightfield_type) GetProcAddress(dll, "hp_drawLightfield");
        hp_setupBuffers = (hp_setupBuffers_type) GetProcAddress(dll, "hp_setupBuffers");
        hp_initialize = (hp_initialize_type) GetProcAddress(dll, "hp_initialize");
        hp_release = (hp_release_type) GetProcAddress(dll, "hp_release");
        load_calibration = (load_calibration_type) GetProcAddress(dll, "load_calibration");
        get_version = (get_version_type) GetProcAddress(dll, "get_version");
	}
    #elif __APPLE__
    void hp_loadLibrary(char * path = (char*)"/Library/HoloPlayAPI.bundle"){
        CFURLRef bundleURL;
        CFBundleRef myBundle;
        bundleURL = CFURLCreateWithFileSystemPath(
            kCFAllocatorDefault,
            CFSTR("/Library/HoloPlayAPI.bundle"),
            kCFURLPOSIXPathStyle,
            true
        );
        myBundle = CFBundleCreate(kCFAllocatorDefault, bundleURL);
        CFRelease(bundleURL);
    
        // make the function pointers
        hp_loadCalibration = (hp_loadCalibration_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_loadCalibration"));
        hp_loadLightfieldShaders = (hp_loadLightfieldShaders_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_loadLightfieldShaders"));
        hp_loadBlitShaders = (hp_loadBlitShaders_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_loadBlitShaders"));
        hp_loadCalibrationIntoShader = (hp_loadCalibrationIntoShader_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_loadCalibrationIntoShader"));
        hp_setupQuiltSettings = (hp_setupQuiltSettings_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_setupQuiltSettings"));
        hp_setupCustomQuiltSettings = (hp_setupCustomQuiltSettings_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_setupCustomQuiltSettings"));
        hp_setupQuiltTexture = (hp_setupQuiltTexture_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_setupQuiltTexture"));
        hp_copyViewToQuilt = (hp_copyViewToQuilt_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_copyViewToQuilt"));
        hp_drawLightfield = (hp_drawLightfield_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_drawLightfield"));
        hp_setupBuffers = (hp_setupBuffers_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_setupBuffers"));
        hp_initialize = (hp_initialize_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_initialize"));
        hp_release = (hp_release_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("hp_release"));
        load_calibration = (load_calibration_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("load_calibration"));
        get_version = (get_version_type) CFBundleGetFunctionPointerForName(myBundle, CFSTR("get_version"));
    }
    #endif
    
#else

extern "C" {
	// quilt settings
	EXPORT_API unsigned int qs_width; 
	EXPORT_API unsigned int qs_height;
	EXPORT_API unsigned int qs_viewWidth; 
	EXPORT_API unsigned int qs_viewHeight;
	EXPORT_API unsigned int qs_rows;
	EXPORT_API unsigned int qs_columns;
	EXPORT_API unsigned int qs_numViews;

	// calibration
	EXPORT_API float cal_pitch;
	EXPORT_API float cal_slope;
	EXPORT_API float cal_center;
	EXPORT_API float cal_viewCone;
	EXPORT_API float cal_invView;
	EXPORT_API float cal_verticalAngle;
	EXPORT_API float cal_DPI;
	EXPORT_API float cal_screenW;
	EXPORT_API float cal_screenH;
	EXPORT_API float cal_flipImageX;
	EXPORT_API float cal_flipImageY;
	EXPORT_API float cal_flipSubp;

	// holoplay stuff
	EXPORT_API unsigned int hp_lightfieldShader;
	EXPORT_API unsigned int hp_blitShader;
	EXPORT_API unsigned int hp_quiltTexture;
	EXPORT_API unsigned int hp_VAO;
	EXPORT_API unsigned int hp_VBO;
	EXPORT_API unsigned int hp_FBO;

	// methods
	EXPORT_API void hp_loadCalibration();
	EXPORT_API void hp_loadLightfieldShaders();
	EXPORT_API void hp_loadBlitShaders();
	EXPORT_API void hp_loadCalibrationIntoShader();
	EXPORT_API void hp_setupQuiltSettings(unsigned int);
	EXPORT_API void hp_setupCustomQuiltSettings(unsigned int, unsigned int, 
		unsigned int, unsigned int, unsigned int, unsigned int, unsigned int); // really annoying
	EXPORT_API void hp_setupQuiltTexture();
	EXPORT_API void hp_copyViewToQuilt(unsigned int);
	EXPORT_API void hp_drawLightfield();
	EXPORT_API void hp_setupBuffers();
	EXPORT_API void hp_initialize();
	EXPORT_API void hp_release();
    EXPORT_API int load_calibration(char *);
    EXPORT_API void get_version(char *);

	// internal shader funcs
	void setBool(unsigned int ID, const std::string &name, bool value);
	void setInt(unsigned int ID, const std::string &name, int value);
	void setFloat(unsigned int ID, const std::string &name, float value);
	void setVector(unsigned int ID, const std::string &name, float value0, float value1, float value2, float value3);
	void setMatrix(unsigned int ID, const std::string &name, float* value);
    void passQuiltSettingsToShader();
}
#endif
