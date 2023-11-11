
# GameApi Builder and meshpage.org

We proudly present a 3d modelling tool, 3d engine and web site for
displaying 3d models.

Our best feature is gltf rendering and its eventual deployment in web, but our
tools are slightly more flexible than just focusing on gltf area.

### download the repo
  git clone https://github.com/terop2/GameApi.git

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

### Choose correct directory
  ```cd GameApi```

### To compile (linux)
  ```make -j 8```

### To compile (emscripten)
 ```./emmake.sh EMSCRIPTEN_PATH=path_to_emscripten_emsdk_directory```

### To compile (windows)
  ```
  set_paths_git.bat
  make.bat
  ```

### Running the software:
  ```
  cd editor
  ./a.out
  ```

### How we normally push our code to hosting space
       (contains fixed ssh paths, which you might need to change):
       ((requires apache + php + emscripten + linux g++ compilers))
 ```# ./full_deploy.sh```

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
 - EMBED: use <embed src="dir/index.html" width="830" height="630"/> to embed the 3d model to your html article
 - ENJOY: open the article in web browser via url

### API access:
    - LINK: link to the libGameApi.so or libGameApi.bc library.
    - CALL:
      ```
      #include "GameApi.hh"
      std::string code= "SCRIPT FROM CODEGEN HERE";
      Env e2;
      EveryApi ev(e);
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

### DEPLOYTOOL ACCESS:
  ```
  cd deploytool
  LD_LIBRARY_PATH=.. ./deploytool --file input_script.txt -o output_zip.zip --homepage=https://tpgames.org
  ```