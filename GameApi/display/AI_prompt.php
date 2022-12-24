<?php

?>
<div class="centre">
<canvas class="canvas" id="canvas" oncontextmenu="event.preventDefault()"></canvas>
</div>
<textarea class="input" id="input" rows="1" cols="50"></textarea>

<style>
html, body { height: 100%; margin: 0; }
.centre { width: 100%; margin-left: 20%; margin-right: 20%; margin-top: 30px; content-align: center; }
.canvas { border-width:0px;border: 5px solid black; border-radius: 10px; background-color: #000000; margin:0; padding:0; width: 820px; height: 620px; }
.input {
  font-size: 200%;
  width: 95%;
  position: absolute;
  bottom: 0;
  margin: 20px 20px 20px 20px;
  padding: 5px 5px 5px 5px;
}
</style>
<script>
var canv = document.getElementById("canvas");
var Module = {
   canvas : canv,
   arguments : [ "--size", "800", "600", "--code", default_script(), "--homepage", "https://tpgames.org/", "--href", window.location.href],
   print : (function() { return function(text) { console.log(text); } })(),
   printErr : (function() { return function(text) { console.log(text); } })(),
   };

var g_emscripten_running = false;

function default_script()
{
   var str="";
   str+="P I1=ev.polygon_api.p_empty();@";
   str+="ML I2=ev.polygon_api.render_vertex_array_ml2(ev,I1);@";
   str+="RUN I3=ev.blocker_api.game_window2(ev,I2,false,false,0.0,100000.0);@";
   return str;
}


function emscripten_loading_callback()
{
}

function emscripten_ready_callback(state)
{
   console.log("Ready");

}

function check_emscripten_ready(state)
{
  if (g_emscripten_running && Module) {
    var value = Module.ccall('get_integer', 'number', ['number'],[1]);
    if (value==2) { emscripten_loading_callback();
    	 setTimeout(function() { check_emscripten_ready(state) }, 100);
    } else
    if (value==1) { emscripten_ready_callback(state); }
    else {
    	 setTimeout(function() { check_emscripten_ready(state) }, 100);
	 }
	 } else
	 {
    	 setTimeout(function() { check_emscripten_ready(state) }, 100);
	 }
}


function check_em() {
    return function() {
	g_emscripten_running = true;
   }
}


var g_first_time=true;
function check_em2_func() {
      	if (g_first_time) {
	   console.log("EMSCRIPTEN_READY");
	   //store.state.appmodel_is_loading="false";
	   //store.state.appmodel_is_model_loading="true";
	   g_first_time=false;
	   check_em()();
	}
}


function check_em2() {
   console.log("DOWNLOAD WAITING..");
   return function() {
      setTimeout(function() { check_em2_func(); },100);
   }
}

function check_if_emscripten_running()
{
  setTimeout(function() { check_emscripten_running() },100);
}
function check_emscripten_running()
{
    var canv = document.getElementById("canvas");
    if (Module) {
	Module['onRuntimeInitialized'] = check_em2();
    } else {
	setTimeout(function() { check_emscripten_running() }, 100);
    }
}


function load_emscripten(state,filename, contents, filenames)
{
    var data2 = "<?php echo $date ?>";
    var agent = navigator.userAgent;
    var mobile = false;
    var firefox = false;
    var idx = 0;
    if (agent.indexOf("Mobile") != -1) mobile = true;
    if ((idx=agent.indexOf("Firefox")) != -1) firefox = true;

    var src = "web_page_highmem.js?"+data2;
    var vstr = agent.substring(idx+8);
    var vnum = parseInt(vstr);

    if (firefox && vnum<=78)
	src="web_page_nothreads.js?" + data2;
    else if (firefox) src="web_page_nothreads_highmem.js?" + data2;
    if (mobile) src="web_page_lowmem.js?"+data2;
    if (!g_emscripten_running) {
      if (filename=="") {
      Module.arguments = [ "--size", "800", "600", "--code", default_script(), "--homepage", "https://tpgames.org/", "--href", window.location.href];
      } else {
      Module.arguments = [ "--size", "800", "600", "--code", convert_enter_to_at(create_script(filename,contents,filenames)), "--homepage", "https://tpgames.org/", "--href", window.location.href];
      }

      var script = document.createElement("script");
      script.setAttribute("src", src);
      document.getElementsByTagName("head")[0].appendChild(script);
      check_if_emscripten_running();
      } else {
      Module.ccall('set_string', null, ['number', 'string'],[0,create_script(filename,contents,filenames)]);
      }
      setTimeout(function() { check_emscripten_ready(state) }, 100);
}

window.onresize = resize_event;
window.setTimeout(function() { resize_event(null); },10);

function resize_event(event)
{
  var wd = window.innerWidth;
  var hd = window.innerHeight;

  wd-=120;
  

  var w = 800;
  var h = 600;

  wd/=3.0;
  wd*=2.0;


  hd-=150;
  //hd/=10.0;
  //hd*=7.5;

  var s_x = wd/w;
  var s_y = hd/h;

  var scale;
  //if (s_x<s_y) scale=s_x; else scale=s_y;
  scale = s_y;
  var mobile = false;
  var agent = navigator.userAgent;
  if (agent.indexOf("Mobile") != -1) mobile = true;

  if (mobile) { scale=wd*3.5/2/w; }

  var scale_x = w*scale;
  var scale_y = h*scale;


  var elem = document.getElementById("canvas");
  elem.style.width = scale_x.toString() + "px";
  elem.style.height = scale_y.toString() + "px";

}

load_emscripten("","");

</script>