

if (!crossOriginIsolated) {
    console.log("NOT CROSSORIGIN ISOLATED => running in lowmem/nothreads mode");
    console.log("Your web server needs the following configuration(usually .htaccess) to get gameapi builder animations working:");
    console.log("Header set Access-Control-Allow-Origin https://meshpage.org");
console.log("Header set Cross-Origin-Embedder-Policy \"require-corp\"");
console.log("Header set Cross-Origin-Resource-Policy \"same-site\"");
console.log("Header set Cross-Origin-Opener-Policy \"same-origin\"");
console.log("Header set Access-Control-Allow-Headers \"Range\"");
console.log("NOTE: you should change https://meshpage.org to your own web hosting location");

    console.log("ADDITIONAL INFO: Your document should have in php the following line:");
    console.log("   header(\"Cross-Origin-Opener-Policy: same-origin\");");

    
}
var pretag = document.getElementById("gameapi_script");
var data = pretag.innerHTML;
var pretag2 = document.getElementById("gameapi_modificationdate");
var prefiletag = document.getElementById("file");
var prefilename = document.getElementById("filename");
var prefiletag2 = document.getElementById("file2");
var prefilename2 = document.getElementById("filename2");
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
    onStartup : function() { check_em()(); },
    canvas : canv,
    arguments : [ "--size", "800", "600", "--code", data4, "--homepage", hom2, "--href", window.location.href, "--deploy"],
    print : (function() { return function(text) { console.log(text); } })(),
};
  Module.locateFile = function(path) { return "engine/" + path+"?"+data2; }

var g_emscripten_running = false;
function load_file()
{
    if (prefiletag) {
        var filename = prefilename.innerText;
        var data3 = atob(prefiletag.innerText);
       var filename2 = prefilename2.innerText;
        var data4 = atob(prefiletag2.innerText);

                                                                         
           try {
               Module.ccall("set_integer", null, ["number", "number"], [2,data3.length], { async:true });
               Module.ccall("set_string", null, ["number", "string"], [1,filename] , { async:true });
               Module.ccall("set_string", null, ["number", "string"], [2,data3], { async:true });

               Module.ccall("set_integer", null, ["number", "number"], [2,data4.length], { async:true });
               Module.ccall("set_string", null, ["number", "string"], [1,filename2], { async:true });
               Module.ccall("set_string", null, ["number", "string"], [2,data4], { async:true });
           } catch(e) {
             console.log(e);
           }
    }
}

var g_check_em_timeout = null;
var g_ready_bit=0; // THIS WILL BE CHANGED BY C++ SIDE                                                                                                  

function check_em() {
    return function() {
        g_emscripten_running = true;
        if (g_check_em_timeout != null)
            clearTimeout(g_check_em_timeout);
        g_check_em_timeout = null;

        //resize_event2(wd,hd,delta_x,delta_y,container_width,container_height,enable_debug_border)(null);
        load_file();
    }
}

function check_emscripten_running()
{
    //console.log("HEP");
    var canv = document.getElementById("canvas");
    if (Module) {
        Module['onRuntimeInitialized'] = check_em();
    g_check_em_timeout = setTimeout(function() {
            console.log("waiting for emscripten startup..");
            if (g_ready_bit==1) {
                console.log("onRuntimeInitialized didn't trigger in time"); check_em()(); } },1000);
    } else {
        setTimeout(function() { check_emscripten_running() }, 100);
    }
}
function check_if_emscripten_running()
{
    //console.log("FOO");
    setTimeout(function() { check_emscripten_running() },100);
}
function load_emscripten()
{
    var agent = navigator.userAgent;
    var mobile = false;
    var firefox = false;
    var idx = 0;
    if (agent.indexOf("Mobile") != -1) mobile = true;
    if ((idx=agent.indexOf("Firefox")) != -1) firefox = true;

    var src = "engine/engine_highmem.js?"+data2;
    var vstr = agent.substring(idx+8);
    var vnum = parseInt(vstr);

    if (firefox && vnum<=78)
        src="engine/engine_lowmem_nothreads.js?" + data2;
    else if (firefox) src="engine/engine_highmem.js?" + data2;
    if (mobile) src="engine/engine_lowmem_nothreads.js?"+data2;
    if (!crossOriginIsolated && !mobile) src="engine/engine_nothreads_highmem.js?"+data2;
    var script = document.createElement("script");
    script.setAttribute("src", src);
    document.getElementsByTagName("head")[0].appendChild(script);
}
load_emscripten();
                                                                        
check_if_emscripten_running();
