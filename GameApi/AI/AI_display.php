<?php
  header("Cross-Origin-Opener-Policy: same-origin");
  header("Access-Control-Allow-Headers: Range");
  ?>
<!DOCTYPE html>
<html>
  <head>
    <meta name="description" content="a 3d model created via meshpage.org/gltf_to_zip.php">
    <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  </head>
  <body>
    <!-- you need to copy the following files to transfer 3d models
         to new web server:
         https://meshpage.org/gameapi_display.zip

	 unzip the package to some new directory in your web host

	 Then change the homepage url from inside pre tag below

	 Then copy-paste a builder codegen script to the gameapi_script 
	 pre tag below, last line needs to have type RUN.
      -->
    <pre id="homepage" style="display:none">
https://meshpage.org/assets/    </pre>
    <pre id="gameapi_script" style="display:none">
PT I1=ev.point_api.point(0.0,0.0,0.0);
P I2=ev.polygon_api.torus2(ev,20,20,I1,250.0,50.0);
MT I3=ev.materials_api.m_def(ev);
MT I4=ev.materials_api.toon_border(ev,I3,4,ffffff00,false);
ML I5=ev.materials_api.bind(I2,I4);
RUN I6=ev.blocker_api.game_window2(ev,I5,false,false,0.0,100000.0);
    </pre>
    <pre id="gameapi_modificationdate" style="display:none">
    <?php
       $files = array("display.html","engine/engine_highmem.wasm",
       "engine/engine_highmem.js",
       "engine/gameapi.js");
       $max_dt = -2147483648;
       foreach($files as $file) {
       $dt = filemtime($file);
       if ($dt!=false && $dt>$max_dt) $max_dt=$dt;
       }
       echo $max_dt;
       ?>
SatJan3119:10:272026
    </pre>
   <div id="container">
    <canvas id="canvas"></canvas>
    </div>
    <style>
      #container { display: inline-block;  width: 400px; height: 300px; }
      #canvas { position: absolute;
		border-width:0px;
		border: 5px solid black;
		border-radius: 10px;
		background-color: #000000;
		margin:0;
		padding:0;
		width: 820px;
		height: 620px;
	      }
      </style>

    <!-- TO CHANGE THE PARAMETERS, YOU SHOULD CHANGE:                                                                                                   
         container width/height (to change what area of the page is reserved for the model)                                                             
         and wd/hd (to change how large the 3d model is)                                                                                                
         and delta_x and delta_y (to position the model)                                                                                                
    -->
    <script>
      let wd = 800;
      let hd = 600;
      let delta_x=0;
      let delta_y=0;
      let container_width=800;
      let container_height=600;
      let enable_debug_border=false;
      if (window.self !== window.top)
      { // we're inside iframe, use iframe dimensions
	  wd = window.innerWidth;
	  hd = window.innerHeight;
	  container_width = window.innerWidth;
	  container_height= window.innerHeight;
      }

      
    </script>
    <script src="engine/gameapi.js?1"></script>
    <script>
function resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border) {
    return function (event) {
      if (window.self !== window.top)
      { // we're inside iframe, use iframe dimensions
	  wd = window.innerWidth;
	  hd = window.innerHeight;
	  container_width = window.innerWidth;
	  container_height= window.innerHeight;
      }
	
          if (Module && g_emscripten_running) {
      var cc = document.getElementById("container");
      var c = document.getElementById("canvas");
          if (enable_debug_border)
              {
                  cc.style.borderStyle="solid";
                  cc.style.borderWidth="1px";
                  cc.style.borderColor="black";
                 c.style.borderStyle="solid";
                  c.style.borderWidth="1px";
                  c.style.borderColor="black";
          }
           try {
               Module.ccall("set_resize_event", null, ["number", "number"], [wd,hd], {async:true});
           } catch(e) {
               console.log(e);
           }
              cc.style.width = (container_width).toString() + "px";
              cc.style.height = (container_height).toString() + "px";
              c.style.width = (wd).toString() + "px";
              c.style.height = (hd).toString() + "px";
              c.style.left = (delta_x + cc.offsetLeft + (cc.clientWidth-wd)/2).toString() + "px";
c.style.top = (delta_y + cc.offsetTop + (cc.clientHeight-hd)/2).toString() + "px";
            const dpr = window.devicePixelRatio || 2;
            c.width = (wd)*dpr;
            c.height = (hd)*dpr;

          } else window.setTimeout(function() { resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border)(null); }, 100);
      }
}

window.onresize=resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border);
window.setTimeout(function() { resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border)(null); },10);

    </script>
  </body>
</html>
