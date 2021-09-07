<?php
header("Cross-Origin-Opener-Policy: same-origin");
?>
<html>
<body>
<script src="https://meshpage.org/vue.js"></script>
<div style="display:flex">
<div id="div2" style="display:none"></div>
<div style=" width:300px; height:300px;background-color:#f84;" id="div1" ondrop="drop(event)" ondragover="allowDrop(event)">
<div style="margin: auto; text-align: center; padding: 120px;">
drag&drop here.
</div>
</div>
<div style="margin-left: 30px; float:left; display:block;">
<h1>The great 3d model viewer</h1>
<h3>Supported file formats</h3>
.gltf, .glb, .stl, .obj, .ply(ascii), .ds
<h3>Supported texture formats</h3>
.jpg, .png
<h3>Features under development</h3>
.mtl, .zip, dir support(OK)
</div>
</div>
<div style="height:10px"></div>
<canvas id="canvas" style="border-width:0px;border: 5px solid black; border-radius: 10px; background-color: #000000; margin:0; padding:0; width: 820px; height: 620px;"></canvas>
<div style="height:10px"></div>

</body>
</html>

<script>
var store = {
   state : {
      empty: true,
      }
      };

var app = new Vue({
   el: '#app',
   data: {
      state: store.state,
      }
   });
   

</script>

<script>
function find_main_item(arr)
{
   var s = arr.length;
   for(var i=0;i<s;i++) {
      if (arr[i].name.substr(-4)==".glb") return i;
      if (arr[i].name.substr(-4)==".stl") return i;
      if (arr[i].name.substr(-5)==".gltf") return i;
      if (arr[i].name.substr(-3)==".ds") return i;
      if (arr[i].name.substr(-4)==".obj") return i;
      if (arr[i].name.substr(-4)==".ply") return i;
   }
   console.log("ERROR: main item not found");
   return -1;
}
function default_script()
{
   var str="";
   str+="P I1=ev.polygon_api.p_empty();@";
   str+="ML I2=ev.polygon_api.render_vertex_array_ml2(ev,I1);@";
   str+="RUN I3=ev.blocker_api.game_window2(ev,I2,false,false,0.0,100000.0);@";
   return str;
}
function convert_enter_to_at(str)
{
  var s = str.length;
  var res = "";
  for(var i=0;i<s;i++) {
    if (str[i]=="\n") res.push("@");
    else res.push(str[i]);
  }
  return res;
}
function get_base_dir(filename)
{
  var res = "";
  var s = filename.split("/");
  var ss = s.length;
  for(var i=0;i<ss-1;i++) {
    res+=s[i];
    if (i!=ss-2) res+="/";
  }
  return res;
}
function find_mtl_name(filenames)
{
   var s = filenames.length;
   for(var i=0;i<s;i++) {
      var filename = filenames[i];
      if (filename.substr(-4)==".mtl") return filenames[i];
   }
   return "";
}
function create_script(filename, contents, filenames)
{
  var res = "";

  var base_dir = get_base_dir(filename);
  var mtl_name = find_mtl_name(filenames);

  if (filename.substr(-4)==".stl") { res+="P I1=ev.polygon_api.stl_load(" + filename + ");\n"; } else
  if (filename.substr(-4)==".obj") {
     if (mtl_name=="") {
       res+="P I1=ev.polygon_api.p_url(ev," + filename + ",350);\n";
       } else {
       res+="P I1=ev.polygon_api.p_mtl(ev," + filename +"," + mtl_name +"," + base_dir + ",600);\n";
       }
     } else
  if (filename.substr(-3)==".ds") { res+="P I1=ev.polygon_api.p_url(ev," + filename + ",350);\n"; } else
  if (filename.substr(-4)==".ply") { res+="P I1=ev.points_api.ply_faces(" + filename + ");\n"; } else
  if (filename.substr(-4)==".glb") { res+="P I1=ev.polygon_api.gltf_load(ev,"+base_dir+"," + filename + ",0,0);\n"; } else
  if (filename.substr(-5)==".gltf") { res+="P I1=ev.polygon_api.gltf_load(ev,"+base_dir+"," + filename + ",0,0);\n"; } else { res+="P I1 = ev.polygon_api.p_empty();\n"; }


  res+="P I2=ev.polygon_api.recalculate_normals(I1);\n";
  res+="MT I3=ev.materials_api.m_def(ev);\n";
  if (filename.substr(-4)==".obj"&&mtl_name!="") {
     res+="MT I4=ev.materials_api.texture_many2(ev,0.5);\n"
  } else
  if (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf") {
     res+="MT I4=ev.materials_api.gltf_material(ev,"+base_dir+"," + filename + ",0,1);\n";
  } else {
     res+="MT I4=ev.materials_api.phong(ev,I3,-0.3,0.3,-1.0,ffff8800,ff666666,5.0);\n";
  }
  res+="ML I5=ev.materials_api.bind(I2,I4);\n";

  if (filename.substr(-4)==".glb" || filename.substr(-5)==".gltf") {
    res+="ML I88=ev.mainloop_api.async_gltf(I5,"+base_dir+"," + filename + ");\n";
    res+="ML I6=ev.mainloop_api.touch_rotate(ev,I88,true,true,0.01,0.01);\n";
  } else {
    res+="ML I6=ev.mainloop_api.touch_rotate(ev,I5,true,true,0.01,0.01);\n";
  }
  res+="RUN I7=ev.blocker_api.game_window2(ev,I6,false,false,0.0,1000000.0);\n";

return res;  
}
function allowDrop(ev)
{
  ev.preventDefault();
}

var contents_array = [];
var filename_array = [];
var g_filename = "";

function extract_contents(file_array,filenames, filename)
{
   g_filename = filename;
   return new Promise((resolve,reject) => {  

   contents_array = [];
   filename_array = [];
   var s = file_array.length;
   var counter = 0;
   for(var i = 0;i<s;i++) {
     var file = file_array[i];
     let fileReader = new FileReader();
     var f = (fileReader,i) => {
     fileReader.onload = () => {
        let fileContents = fileReader.result;
	var binary = '';
	var bytes = new Uint8Array( fileContents );
	//console.log(bytes.length);
	var len = bytes.byteLength;
	for(var j=0;j<len;j++) {
	  binary+= String.fromCharCode(bytes[j]);
	}
	//console.log(binary.length);
	contents_array.push(binary);
	filename_array.push(fix_filename(filenames[i]));
	counter++;
	if (counter==s) resolve("success");
	}
     };
     f(fileReader,i);
     fileReader.readAsArrayBuffer(file);
  }
});
}

function load_finished(value)
{
   load_files(contents_array,filename_array);
   load_emscripten(g_filename, contents_array, filename_array);
}
function fix_filename(filename)
{
  var res = "";
  var s = filename.length;
  for(var i=0;i<s;i++) {
     if (filename[i]==' ') res+="_"; else 
     if (filename[i]=='\\') res+="/"; else res+=filename[i];
  }
  return res;
}
function traverseFileTree(item, path) {
   return new Promise( resolve => {
   path = path || "";
   if (item.isFile) {
      item.file(function(file) {
         //console.log("File:", path + file.name);
	 resolve({file: file,path: path+file.name});
	 });
    } else if (item.isDirectory) {
          var res = [];
	  var names = [];
	  var dirReader = item.createReader();
	  dirReader.readEntries(function(entries) {
	     var promises = [];
	     for(var i=0;i<entries.length;i++) {
	        const promise = traverseFileTree(entries[i],path+item.name + "/");
		promises.push(promise);		
		}
		resolve(Promise.all(promises));
		});
	}
	});
}
function flatten_arrays(data)
{
   var res = [];
   var names = [];
   if (Array.isArray(data)) {
     var s = data.length;
     for(var i=0;i<s;i++) {
        const [res2,names2] = flatten_arrays(data[i]);
	for(var k=0;k<res2.length;k++) {
	   res.push(res2[k]);
	   names.push(names2[k]);
	}
     }
     return [res,names];
   }
   return [[data.file],[data.path]];
}
function drop(ev)
{
  ev.preventDefault();
  var files = [];
  var filenames = [];
  if (ev.dataTransfer.items) {
     const promise2 = new Promise( resolve => {

     	   var s = ev.dataTransfer.items.length;
	   var promises = [];
     	   for(var i=0;i<s;i++) {
       	   	   var item = ev.dataTransfer.items[i].webkitGetAsEntry();
       		   if (item) {
		   promises.push(traverseFileTree(item));
	  	   }
		   }
	    resolve(Promise.all(promises));
       });
       promise2.then( data => {
           const [files,filenames] = flatten_arrays(data);
	   //console.log(files);
	   //console.log(filenames);
           var main_item_num = find_main_item(files);
	   var main_item = files[main_item_num];
           var main_item_name = filenames[main_item_num];

           const promise = extract_contents(files,filenames,fix_filename(main_item_name));
           promise.then(load_finished);

     });
     }
}

</script>

<script>
var canv = document.getElementById("canvas");
var Module = {
   canvas : canv,
   arguments : [ "--size", "800", "600", "--code", default_script(), "--homepage", "", "--href", window.location.href],
   print : (function() { return function(text) { console.log(text); } })(),
   };


function load_emscripten(filename, contents, filenames)
{
    var data2 = "226679";
    var agent = navigator.userAgent;
    var mobile = false;
    var firefox = false;
    var idx = 0;
    if (agent.indexOf("Mobile") != -1) mobile = true;
    if ((idx=agent.indexOf("Firefox")) != -1) firefox = true;

    var src = "web_page.js?"+data2;
    var vstr = agent.substring(idx+8);
    var vnum = parseInt(vstr);

    if (firefox && vnum<=78)
	src="web_page_nothreads.js?" + data2;
    else if (firefox) src="web_page_nothreads_highmem.js?" + data2;
    if (mobile) src="web_page_lowmem.js?"+data2;
    if (!g_emscripten_running) {
      if (filename=="") {
      Module.arguments = [ "--size", "800", "600", "--code", default_script(), "--homepage", "", "--href", window.location.href];
      } else {
      Module.arguments = [ "--size", "800", "600", "--code", convert_enter_to_at(create_script(filename,contents,filenames)), "--homepage", "", "--href", window.location.href];
      }

      var script = document.createElement("script");
      script.setAttribute("src", src);
      document.getElementsByTagName("head")[0].appendChild(script);
      check_if_emscripten_running();
      } else {
      Module.ccall('set_string', null, ['number', 'string'],[0,create_script(filename,contents,filenames)]);
      }
}

function load_files(data_array, filename_array)
{
  var s2 = data_array.length;
  for(var i=0;i<s2;i++) {
    var data3 = data_array[i];


    var filename = filename_array[i];
    var sz = data3.length;

    Module.ccall('set_string', null, ['number', 'string'], [1,filename], {async:true} );

    for(var s=0;s<sz;s+=40960) {
      var use_sz = 40960;
      if (s>=sz-40960) {
        use_sz=sz-s;
      }
    Module.ccall('set_integer', null, ['number', 'number'], [2,use_sz], {async:true} );

    var buffer = "";
    var index = "0123456789abcdef";
    for(var o = 0;o<use_sz;o++) {
        var low = data3[s+o].charCodeAt(0)&0x0f;
	var high = (data3[s+o].charCodeAt(0)&0xf0)>>>4;
	buffer+=index[high];
	buffer+=index[low];
    }
    //for(var c = 0;c<15;c++) console.log(buffer[c]);

    Module.ccall('set_string', null, ['number', 'string'], [3,buffer], {async:true} );
    }
    Module.ccall('set_string', null, ['number', 'string'], [4,""], {async:true} );

  }
}

var g_emscripten_running = false;
function check_em() {
    return function() {
	g_emscripten_running = true;
	//resize_event(null);
	//load_file();
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

load_emscripten("");

</script>
