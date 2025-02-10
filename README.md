
# GameApi Builder and meshpage.org

We proudly present a 3d modelling tool, 3d engine and web site for
displaying 3d models.

Our best feature is gltf rendering and its eventual deployment in web, but our
tools are slightly more flexible than just focusing on gltf area. The software
uses 8 cpus in the following locations:
1) vertex arrays,
2) textures in sprites,
3) url loading,
4) bitmap loading,
5) zip decoding,
6) points,
7) sprites,
8) gltf bitmaps decoding from png/jpeg files

<img src="https://meshpage.org/assets/builder_mini.png" width="320px" height="300px"/>
<img src="https://meshpage.org/assets/sanmiguel.png" width="320px" height="300px"/>
<img src="https://meshpage.org/assets/suitcase.png" width="320px" height="300px"/>

### Android runtime

  https://meshpage.org/gameapi_xmas.apk

  (given that android security horror prevents changing script.txt file from
  within the apk file, you'll need someone who has full ndk installed and build
  of gameapi near. Thus, if you have designed a script with gameapi builder and
  would want it to work on android, send the codegen script to
  terop@kotiposti.net as email, and I'll install it to apk file and send
  you the resulting apk in return mail.)

### Change your web page
```
<img src="xxx.jpg"/>

to

<iframe scrolling="no" src="xxxzipdir/display.html" width="830" height="630"/>
```

You can find the display.html from the generated .zip files.

### Download the repo
  - git clone https://github.com/terop2/GameApi.git

### Dependent packages (ubuntu)
  - apt-get install libsdl2-dev
  - apt-get install libfreetype6-dev
  - apt-get install libsdl2-mixer-dev
  - apt-get install libglew-dev
  - apt-get install curl
  - apt-get install libstdc++0
  - apt-get install libc6
  - apt-get install libgcc-s1
  - apt-get install unzip
  - apt-get install zip
  - apt-get install tar


### Compiling
```
Choose correct directory:
  cd GameApi

To compile (linux):
  make clean     // only in full build
  make -j 8

To compile (emscripten):
 make -f Makefile.LinuxEm clean  // only in full build
 ./emmake.sh EMSCRIPTEN_PATH=path_to_emscripten_emsdk_directory

To compile (windows):
  set_paths_git.bat
  make.bat

Running the software:
  cd editor
  ./a.out

How we normally push our code to hosting space:
  # ./full_deploy.sh
or
  # full_deploy_win32.bat

Android cmdline: (but requires sdl2 and freetype and libcurl compilation)
                 (current status: at least quake_ml[|2|3] are broken.)
  1) SDL2 compilation:
   export ANDROID_NDK=/home/terop/Android/Sdk/ndk/28.0.12674087
   cd androiddeps/SDLBuild[|2|3|4]
   ./cmake.cmd
    make
 2) freetype compilation:
  cd androiddeps/freetype-2.13.3/build
  emacs android_configure.sh
  (should compile to all 4 platforms:
     aarch64-linux-android30-clang++, --host=aarch64-linux-android
     i686-linux-android30-clang++, --host=i686-linux-android
     x86_64-linux-android30-clang++, --host=x86_64-linux-android
     armv7a-linux-androideabi30-clang++, --host=arm-linux-androideabi
     )
  make
 3) libcurl compilation
    git clone https://github.com/Microsoft/vcpkg.git
    ./vcpkg/bootstrap-vcpkg.sh
    ./vcpkg/vcpkg install curl:arm64-android
    ./vcpkg/vcpkg install curl:arm-android
    ./vcpkg/vcpkg install curl:x64-android
    ./vcpkg/vcpkg install curl:x86-android
    (then headers are in ./vcpkg/installed/arm64-android/include)
    (then libs are in ./vcpkg/installed/arm64-android/lib)
 4) Changing paths for sdl2, ndk, sdk and freetype in Makefile.android, cmdline4/build.sh and cmdline4/build_package.sh
 5) Compile package + push it to android device:
  make -f Makefile.android clean
  make -f Makefile.android -j 8
  (cd cmdline4; ./ammake.sh)


```

### Important urls:
  - https://meshpage.org                  (main web site)
  - https://meshpage.org/meshpage_4       (download newest builder tool binaries)
  - https://meshpage.org/view.php         (3d model viewer)
  - https://meshpage.org/gltf_to_zip.php  (gltf to html5 zip converter)
  - https://meshpage.org/meshpage_5       (FAQ)
  
### Deploy Process#1:
 - CREATE: use our technology to create the 3d model display
 - DEPLOY: get a .zip file from gltf_to_zip converter, builder tool(save_deploy) or meshpage.org web site
 - UNZIP: unzip the zip file to a new directory in your hosting space
 - EMBED: use `<iframe src="xxxdir/index.html" width="830" height="630"/>` to embed the 3d model to your html article
 - ENJOY: open the article in web browser via url

### API access:
    - LINK: link to the libGameApi.so or libGameApi.bc library.
    - CALL:
    
      ```
      #include "GameApi.hh"
      tasks_init();

      std::string code= "SCRIPT FROM CODEGEN HERE";
      Env e2;
      EveryApi ev(e2);
      ExecuteEnv e;
      ev.mainloop_api.init_window(w_width,w_height,"GameApi", vr_init);
      ev.mainloop_api.set_screen_size(w_width, w_height);
      ev.mainloop_api.set_homepage_url(homepageurl);
      ev.mainloop_api.set_seamless_url(seamless_url);
      ev.shader_api.load_default();
      std::pair<int,std::string> blk = GameApi::execute_codegen(e2, ev, code, e);
      if (blk.second == "RUN") {
      	 RUN r;
    	 r.id = blk.first;
    	 ev.blocker_api.run2(ev,r);
       } else if (blk.second=="OK") {
         BLK b;
         b.id = blk.first;
         ev.blocker_api.run(b);
       } else {
         std::cout << "ERROR: internal error" << std::endl;
       }
       ```

### Deploytool access:
  ```
  cd deploytool
  LD_LIBRARY_PATH=.. ./deploytool --file input_script.txt -o output_zip.zip --homepage=https://meshpage.org/assets/
  ```

### Cmdline access:
  ```
  cd cmdline
  LD_LIBRARY_PATH=.. ./gameapi_cmdline --file input_script.txt
  ```

### License:

LGPL/GPL

Yours,

<img src="https://meshpage.org/assets/avatar.png" width="80px" height="80px"/>
