<?php
header("Cross-Origin-Opener-Policy: same-origin");
$date = filemtime("web_page.js");
?>
<html>
<body>
<script src="https://meshpage.org/vue.js"></script>
<div style="display:flex">
<div id="div2" style="display:none"></div>
<div style=" width:300px; height:300px;background-color:#f84;" id="div1" ondrop="drop(event)" ondragover="allowDrop(event)">
<div style="margin: auto; text-align: center; padding: 120px;" id="label">
drag&drop here.
</div>
</div>
<div style="margin-left: 30px; float:left; display:block;">
<h1>The great 3d model viewer</h1>
<h3>Supported file formats</h3>
.gltf, .glb, .stl, .obj, .mtl, .ds, dirs
<h3>Supported texture formats</h3>
.jpg, .png
<h3>Features under development</h3>
.zip
</div>
<div style="margin-left: 30px; float:left; display:block;">
<p>
Material:
<select name="material" id="material-select" onchange="drop2()">
  <option value="-1">Default</option>
  <option value="0">EdgedPhong</option>
  <option value="1">EdgedMetal</option>
  <option value="2">EdgedGold</option>
  <option value="3">SmoothPhong</option>
  <option value="4">SmoothMetal</option>
  <option value="5">SmoothGold</option>
  <option value="6">Wood1</option>
  <option value="7">Wood2</option>
  <option value="8">Wood3</option>
  <option value="9">Shiny Wood1</option>
  <option value="10">Shiny Wood2</option>
  <option value="11">Shiny Wood3</option>
</select><br><p>
Background:<select name="background" id="background-select" onchange="drop2()">
  <option value="-1">Black</option>
  <option value="0">White</option>
  <option value="1">Grey</option>
</select><br><p>
Border:<select name="border" id="border-select" onchange="drop2()">
  <option value="-1">None</option>
  <option value="0">Black 2px</option>
  <option value="1">White 2px</option>
  <option value="2">Black 4px</option>
  <option value="3">White 4px</option>
  <option value="4">Black 8px</option>
  <option value="5">White 8px</option>
  </select><br><p>
Model:<select name="model" id="model-select" onchange="drop2()">
 <option value="-1">(use drag&drop area)</option>
 <option value="0">Sheep</option>
 <option value="1">BoomBox</option>
 <option value="2">Duck</option>
 <option value="3">Astronaut</option>
 </select>
</div>
</div>
<div style="height:10px"></div>
<canvas id="canvas" style="border-width:0px;border: 5px solid black; border-radius: 10px; background-color: #000000; margin:0; padding:0; width: 820px; height: 620px;" oncontextmenu="event.preventDefault()"></canvas>
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
      //if (arr[i].name.substr(-4)==".ply") return i;
   }
   set_label("ERROR: main item not found");
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
function get_model_value()
{
  var elem = document.getElementById("model-select");
  return parseInt(elem.value);
}
function get_model(i)
{
   var model = "";
   if (i==0) model="https://tpgames.org/wooly_sheep.stl";
   if (i==1) model="https://tpgames.org/BoomBox.glb";
   if (i==2) model="https://tpgames.org/Duck.glb";
   if (i==3) model="https://tpgames.org/Astronaut.glb";
   return model;
}
function get_border_value()
{
  var elem = document.getElementById("border-select");
  return parseInt(elem.value);
}
function get_border(i,m)
{
  var color = "000000";
  if (i==1) color="ffffff";
  if (i==3) color="ffffff";
  if (i==5) color="ffffff";

  var width = "0";
  if (i==0 || i==1) width="2";
  if (i==2 || i==3) width="4";
  if (i==4 || i==5) width="8";

  var variable = "I2";
  if (m==-1) variable="I1";

  var res = "";
  res+= "P I205=ev.polygon_api.recalculate_normals(" + variable + ");\nP I206=ev.polygon_api.smooth_normals2(I205);\n"
  //res+= "MT I401=ev.materials_api.m_def(ev);\n"
  res+= "MT I501=ev.materials_api.toon_border(ev,I4," + width + ",ff" + color + ");\n";
  res+="ML I502=ev.materials_api.bind(I206,I501);\n";
  //res+="ML I502=ev.mainloop_api.depthfunc(I5022,0);\n";
  return res;
}
function get_background_value()
{
  var elem = document.getElementById("background-select");
  return parseInt(elem.value);
}
function get_background(i)
{
  var color = "000000";
  if (i==0) color="ffffff";
  if (i==1) color="888888";
  var background="BM I41=ev.bitmap_api.newbitmap(100,100,ff" + color + ");\nBM I42=ev.bitmap_api.scale_bitmap_fullscreen(ev,I41);\nML I43=ev.sprite_api.vertex_array_render(ev,I42);\nML I44=ev.sprite_api.turn_to_2d(ev,I43,0.0,0.0,800.0,600.0);\n";
  return background;
}

function get_material_value()
{
  var elem = document.getElementById("material-select");
  return parseInt(elem.value);
}
function get_material(i)
{

// these are all mat :: () -> I4 :: () -> MT
var phongmaterial = "MT I4=ev.materials_api.phong(ev,I3,-0.3,0.3,-1.0,ffff8800,ff666666,5.0);\n";
var metalmaterial = "MT I4=ev.materials_api.gltf_material3(ev,0.5,0.8,1,1,1,1,1);\n";
var goldmaterial = "MT I4=ev.materials_api.gltf_material3(ev,0.5,0.97,0.5,0.3,0,1,1);\n";
var wood1material = "BM I500=ev.bitmap_api.loadbitmapfromurl(http://tpgames.org/wood1.png);\nMT I400=ev.materials_api.texture(ev,I500,1.0);\nMT I401=ev.materials_api.m_def(ev);\nMT I402=ev.materials_api.phong(ev,I401,-0.3,0.3,-1.0,ffff8800,ff666666,5.0);\nMT I4=ev.materials_api.combine_materials(ev,I402,I400);\n";

var wood2material = "BM I500=ev.bitmap_api.loadbitmapfromurl(http://tpgames.org/wood2.jpg);\nMT I400=ev.materials_api.texture(ev,I500,1.0);\nMT I401=ev.materials_api.m_def(ev);\nMT I402=ev.materials_api.phong(ev,I401,-0.3,0.3,-1.0,ffff8800,ff666666,5.0);\nMT I4=ev.materials_api.combine_materials(ev,I402,I400);\n";


var wood3material = "BM I500=ev.bitmap_api.loadbitmapfromurl(http://tpgames.org/wood3.jpg);\nMT I400=ev.materials_api.texture(ev,I500,1.0);\nMT I401=ev.materials_api.m_def(ev);\nMT I402=ev.materials_api.phong(ev,I401,-0.3,0.3,-1.0,ffff8800,ff666666,5.0);\nMT I4=ev.materials_api.combine_materials(ev,I402,I400);\n";



//var wood2material = "BM I500=ev.bitmap_api.loadbitmapfromurl(http://tpgames.org/wood2.jpg);\nMT I400=ev.materials_api.texture(ev,I500,0.7);\nMT I4=ev.materials_api.phong(ev,I400,-0.3,0.3,-1.0,ffff8800,ff666666,5.0);\n";
//var wood3material = "BM I500=ev.bitmap_api.loadbitmapfromurl(http://tpgames.org/wood3.jpg);\nMT I400=ev.materials_api.texture(ev,I500,0.7);\nMT I4=ev.materials_api.phong(ev,I400,-0.3,0.3,-1.0,ffff8800,ff666666,5.0);\n";

var woodfile = "wood1.png";
if (i==10) woodfile="wood2.jpg";
if (i==11) woodfile="wood3.jpg";

var glow = "30";
if (i==10) glow="300";
if (i==11) glow="3000";

var shinywood = "BM I111=ev.bitmap_api.loadbitmapfromurl(http://tpgames.org/" + woodfile + ");\nFB I222=ev.float_bitmap_api.from_red(I111);\nMT I333=ev.materials_api.bump_phong(ev,-0.3,0.3,-1,ff000000,ffffffff," + glow + ",I222,15);\nMT I555=ev.materials_api.texture(ev,I111,0.7);\nMT I4=ev.materials_api.combine_materials(ev,I333,I555);\n";



// these are all I1 => I2 :: P->P
var texcoords = "P I2=ev.polygon_api.texcoord_plane(I1,-600,600,-600,600);\n";
var normals = "P I2=ev.polygon_api.recalculate_normals(I1);\n";
var smoothnormals = "P I200=ev.polygon_api.recalculate_normals(I1);\nP I2=ev.polygon_api.smooth_normals2(I200);\n";
var texcoord_normals = "P I200=ev.polygon_api.texcoord_plane(I1,-600,600,-600,600);\nP I2=ev.polygon_api.recalculate_normals(I200);\n"
var texcoord_smoothnormals = "P I200=ev.polygon_api.texcoord_plane(I1,-600,600,-600,600);\nP I201=ev.polygon_api.recalculate_normals(I200);\nP I2=ev.polygon_api.smooth_normals2(I201);\n"

if (i==-1) return ["",""];
if (i==0) return [phongmaterial,normals];
if (i==1) return [metalmaterial,normals];
if (i==2) return [goldmaterial,normals];
if (i==3) return [phongmaterial,smoothnormals];
if (i==4) return [metalmaterial,smoothnormals];
if (i==5) return [goldmaterial,smoothnormals];
if (i==6) return [wood1material,texcoord_normals];
if (i==7) return [wood2material,texcoord_normals];
if (i==8) return [wood3material,texcoord_normals];
if (i==9) return [shinywood,texcoord_smoothnormals];
if (i==10) return [shinywood,texcoord_smoothnormals];
if (i==11) return [shinywood,texcoord_smoothnormals];
return [phongmaterial,normals];
}
function create_script(filename, contents, filenames)
{
  var res = "";

  var base_dir = get_base_dir(filename);
  var mtl_name = find_mtl_name(filenames);

  var material_value = get_material_value();
  var material = get_material(material_value);

  var background_value = get_background_value();
  var background = get_background(background_value);

  var border_value = get_border_value();
  var border = get_border(border_value,material_value);

  if (filename.substr(-4)==".stl") { res+="P I17=ev.polygon_api.stl_load(" + filename + ");\nP I18=ev.polygon_api.recalculate_normals(I17);\nP I19=ev.polygon_api.color_from_normals(I18);\nP I1=ev.polygon_api.color_grayscale(I19);\n";
     } else
  if (filename.substr(-4)==".obj") {
     if (mtl_name=="") {
       res+="P I1=ev.polygon_api.p_url(ev," + filename + ",350);\n";
       } else {
       res+="P I1=ev.polygon_api.p_mtl(ev," + filename +"," + mtl_name +"," + base_dir + ",600);\n";
       }
     } else
  if (filename.substr(-3)==".ds") { res+="P I1=ev.polygon_api.p_url(ev," + filename + ",350);\n"; } else
  //if (filename.substr(-4)==".ply") { res+="P I1=ev.points_api.ply_faces(" + filename + ");\n"; } else
  if (filename.substr(-4)==".glb") { res+="P I1=ev.polygon_api.gltf_load(ev,"+base_dir+"," + filename + ",0,0);\n"; } else
  if (filename.substr(-5)==".gltf") { res+="P I1=ev.polygon_api.gltf_load(ev,"+base_dir+"," + filename + ",0,0);\n"; } else
     {
	res+="P I1=ev.polygon_api.cube(-300,300,-300,300,-300,300);\n";
	}

  if (material[0]!="") {
     res+=material[1];
  } else
  if (!((filename.substr(-4)==".obj"&&mtl_name!="")||filename.substr(-4)==".glb"||filename.substr(-5)==".gltf")) {
      res+="P I2=ev.polygon_api.recalculate_normals(I1);\n";
      }
  res+="MT I3=ev.materials_api.m_def(ev);\n";
  if (material[0]!="") {
     res+=material[0];
  } else
  if (filename.substr(-4)==".obj"&&mtl_name!="") {
     res+="MT I4=ev.materials_api.texture_many2(ev,0.5);\n"
  } else
  if (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf") {
     res+="MT I4=ev.materials_api.gltf_material(ev,"+base_dir+"," + filename + ",0,1);\n";
  } else {
     //res+="MT I4=ev.materials_api.colour_material(ev,0.9);\n";
     res+="MT I4=ev.materials_api.vertex_phong(ev,I3,-0.3,0.3,-1.0,ffff8800,ff666666,5.0,0.5);\n";
  }


  if (!((filename.substr(-4)==".obj"&&mtl_name!="")||filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"))
     res+="ML I6=ev.materials_api.bind(I2,I4);\n";
  else
     res+="ML I6=ev.materials_api.bind(I1,I4);\n";


  if ((parseInt(material_value)==0&&parseInt(border_value)==0) && (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf")) {
     res="ML I6=ev.mainloop_api.gltf_mesh_all(ev," + base_dir +"," + filename + ");\n";

  }
  res+=border; // outputs I502
  
  res+="ML I66=ev.mainloop_api.array_ml(ev,std::vector<ML>{I6,I502});\n";
  //res+="ML I68=ev.mainloop_api.isometric(I66,0.5236,0.5236,-1400.0);\n";


  if (filename.substr(-4)==".glb" || filename.substr(-5)==".gltf") {
    res+="ML I88=ev.mainloop_api.async_gltf(I66,"+base_dir+"," + filename + ");\n";
    res+="ML I7=ev.mainloop_api.touch_rotate(ev,I88,true,true,0.01,0.01);\n";
  } else {
    res+="ML I7=ev.mainloop_api.touch_rotate(ev,I66,true,true,0.01,0.01);\n";
  }
  res+="ML I8=ev.mainloop_api.mouse_roll_zoom(ev,I7);\n";
  res+="ML I9=ev.mainloop_api.right_mouse_pan(ev,I8);\n";
  res+=background;
  res+="ML I14=ev.mainloop_api.array_ml(ev,std::vector<ML>{I44,I9});\n";
  //res+="ML I15=ev.mainloop_api.perspective(ev,I14,90.0,10.1,60000.0);\n";
  res+="RUN I10=ev.blocker_api.game_window2(ev,I14,false,false,0.0,1000000.0);\n";


return res;  
}
function allowDrop(ev)
{
  ev.preventDefault();
}
function preventDefault(ev)
{
   if (ev.button==2)
      ev.preventDefault();
}

var contents_array = [];
var filename_array = [];
var g_filename = "";

function extract_contents(file_array,filenames, filename)
{
   g_filename = filename;

   if (file_array==""||filenames=="") {
      return new Promise((resolve,reject) => {
           resolve("success");
      });
   }

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
   set_label("Drag & Drop files here..");
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
var old_main_item_name = "";
var old_files = "";
var old_filenames = "";
function drop2()
{
  var model_val = get_model_value();
  var model = get_model(model_val);
  if (old_main_item_name!="" || model!="") {
    if (model!="") { old_main_item_name=model; }
    // old_files, old_filenames
const promise = extract_contents("","",fix_filename(old_main_item_name));
    promise.then(load_finished);
    }
}

function drop(ev)
{
  set_label("Loading model..");

  var mod = document.getElementById("model-select");
  mod.value="-1";


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

	   old_files = files;
	   old_filenames = filenames;
	   old_main_item_name = main_item_name;

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
   arguments : [ "--size", "800", "600", "--code", default_script(), "--homepage", "https://tpgames.org/", "--href", window.location.href],
   print : (function() { return function(text) { console.log(text); } })(),
   };


function load_emscripten(filename, contents, filenames)
{
    var data2 = "<?php echo $date ?>";
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
	set_label("Drag & Drop files here..");
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
function set_label(label)
{
   var wid = document.getElementById("label");
   wid.innerHTML = label;
}
set_label("Loading 3d engine..");
load_emscripten("");

</script>
