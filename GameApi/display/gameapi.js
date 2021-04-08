
var pretag = document.getElementById("gameapi_script");
var data = pretag.innerHTML;
var pretag2 = document.getElementById("gameapi_modificationdate");
var data2 = pretag2.innerHTML.trim();
var lines = data.split("\n");
var lines2 = "";
for(var i = 0;i<lines.length;i++) {
    lines2 = lines2.concat(lines[i].trim()+"@");
}
var data4 = lines2.replace(/\t/g,"");
var canv = document.getElementById("canvas");
var hom = document.getElementById("homepage");
var hom2 = hom.innerHTML;
var Module = {
    canvas : canv,
    arguments : [ "--size", "800", "600", "--code", data4, "--homepage", hom2],
    print : (function() { return function(text) { console.log(text); } })(),
};
load_emscripten();
window.onresize = resize_event;
window.setTimeout(function() { resize_event(null); },10);
check_if_emscripten_running();
var g_emscripten_running = false;
function check_em() {
    return function() {
	g_emscripten_running = true;
	resize_event(null);
    }
}


function check_emscripten_running()
{
    var canv = document.getElementById("canvas");
    if (Module) {
	Module['onRuntimeInitialized'] = check_em();
    } else {
	setTimeout(function() { check_emscripten_running() }, 100);
    }
}
function check_if_emscripten_running()
{
    setTimeout(function() { check_emscripten_running() },100);
}

function resize_event(event) {
  var wd = window.innerWidth;
  var hd = window.innerHeight;
  var wd2 = wd/800.0;
  var hd2 = hd/600.0;
  if (wd2>hd2) wd2=hd2; else
  if (hd2>wd2) hd2=wd2;

  var hd3 = hd2;

  wd2*=800.0;
  hd2*=600.0;
  wd=wd2;
  hd=hd2;
    wd-=60;
    hd-=60;
  var iframe = document.getElementById("canvas");
  if (!iframe) return;
  iframe.style.width = (wd).toString() + "px";
  iframe.style.height = (hd).toString() + "px";

  //var prog = document.getElementById("prgress");
  //prog.style.width = (wd).toString() + "px";
  //var hdd = 30*hd3;
//prog.style.height = hdd.toString() + "px";

  if (Module && g_emscripten_running) {
	   try {
Module.ccall('set_resize_event', null, ['number', 'number'], [wd,hd], {async:true});
	   } catch(e) {
	     console.log(e);
	   }

  }

}

function load_emscripten()
{
    var agent = navigator.userAgent;
    var mobile = false;
    var firefox = false;
    if (agent.indexOf("Mobile") != -1) mobile = true;
    if (agent.indexOf("Firefox") != -1) firefox = true;

    var src = "web_page.js?"+data2;
    if (firefox) src="web_page_nothreads.js?" + data2;
    if (mobile) src="web_page_lowmem_nothreads.js?"+data2;
    
    var script = document.createElement("script");
    script.setAttribute("src", src);
    document.getElementsByTagName("head")[0].appendChild(script);
}
