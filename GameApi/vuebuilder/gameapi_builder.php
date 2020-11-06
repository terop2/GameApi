<?php

?>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, shrink-to-fit=no">
</head>
<body id="body">
<script src="https://meshpage.org/vue.js"></script>
<div id="app">
<div class="row">
<div class="column side" id="sidecolumn">
<div class="scrollarea" id="scrollarea">
<div class="space"></div>
<?php
$array = array("bitmap", "polygon", "boolbitmap", "floatbitmap", "materials", "lines", "points", "point", "volume", "floatvolume", "font", "texture", "booleanops", "move", "waveform", "mainloop", "framebuffer");
$array2 = array("__BitmapApi", "__PolygonApi", "__BoolBitmapApi", "__FloatBitmapApi", "__MaterialsApi", "__LinesApi", "__PointsApi", "__PointApi", "__VolumeApi", "__FloatVolumeApi", "__FontApi", "__TextureApi", "__BooleanOps", "__MoveApi", "__WaveformApi", "__MainLoopApi", "__FrameBufferApi");
$array3 = array("Bitmap", "Polygon", "BoolBitmap", "FloatBitmap", "Material", "Lines", "Points", "Point", "Volume", "FloatVolume", "Font", "Texture", "BooleanOps", "Move", "Waveform", "MainLoop", "FrameBuffer");

$i=0;
foreach($array as $value)
{
$api = $array2[$i];
$truncapi = substr($api, 2);
$display = $array3[$i];
$i = $i + 1;
?>
<template v-for="msg in <?php echo $value?>data">
<template v-if="msg[0]=='_'">
<div>
<button v-if="msg == '<?php echo $api ?>'" onclick="store.flip('<?php echo $value ?>'); return false" class="highlight noselect"><?php echo $display ?></button>
</div>
</template>
<template v-else>
<div v-show="state.<?php echo $value ?>" v-on:click="select_box($event)" class="label noselect" v-on:mousemove="menumousemove($event)">{{ msg }}</div>
</template>
</template>
<?php
}
?>
<div class="space"></div>
</div>
</div>
<div class="column middle" id="canvascolumn">
<div class="canvas" id="divcanvas">
<canvas oncontextmenu="return false;" id="myCanvas" width="10000" height="10000"  v-box="boxes"  v-link="links" v-tempbox="tempboxes" v-popup="popups" v-popuphighlight="popuphighlight" v-popuptext="popuptexts" v-templine="templines" v-on:mousedown="press($event)" v-on:mouseup="release($event)" v-on:mousemove="mousemove($event)" v-on:keydown.ctrl.83.prevent.stop="ctrl_s_handler($event)"  ></canvas>
</div>
<div class="display" id="display" style="display:none">
</div>
<div class="tooltip noselect" id="tooltip"></div>
<div class="properties" id="questionaire">
  <div style="display:none" class="table" id="qtable">
  <template v-for="line in questionaire">
  <div class="tablerow">
  <div class="tablecolumn">
  {{ line.name }}:
  </div><div class="tablecolumn2">
  <div class="edit"><input type="text" class="input" v-model="line.value"></input></div>
  </template>
   <div class="tablerow">
 <div class="tablecolumn"></div>
 <div class="tablecolumn2">
  <button style="display:none" id="questionaireclose" class="close2" v-on:click="close_questionaire($event)">Close</button>
  </div>
  </div>
 </div>
</div>
</div>
</div>
<div class="properties" id="properties">
  <div style="display:none" class="table" id="table">
  <template v-for="line in properties">
  <div class="tablerow">
  <div class="tablecolumn">
    {{ line.name }}:
    </div><div class="tablecolumn2">
    <div class="edit"><input type="text" class="input" v-model="line.value"></input></div>
   </div></div>
<!--  {{ line.id }} {{ line.num }} {{ line.name }} {{ line.type }} {{ line.value }} -->
 </template>
 <div class="tablerow">
 <div class="tablecolumn"></div>
 <div class="tablecolumn2">
  <button style="display:none" id="propertyclose" class="close2" v-on:click="close_properties($event)">Close</button>
  </div>
  </div>
 </div>
</div>
</div>
</div>
</div>
</div>
<script>
var store = {
  state: {
    seen: false,
    bitmap: false,
    polygon: false,
    boolbitmap: false,
    floatbitmap: false,
    materials: false,
    lines: false,
    points: false,
    point: false,
    volume: false,
    floatvolume: false,
    font: false,
    texture: false,
    booleanops: false,
    move: false,
    waveform: false,
    mainloop: false,
    framebuffer: false
  },
  flipSeen() { this.state.seen = !this.state.seen },
  flip(a) {
    if (a=='bitmap') this.state.bitmap = !this.state.bitmap;
    if (a=='polygon') this.state.polygon = !this.state.polygon;
    if (a=='boolbitmap') this.state.boolbitmap = !this.state.boolbitmap;
    if (a=='floatbitmap') this.state.floatbitmap = !this.state.floatbitmap;
    if (a=='materials') this.state.materials = !this.state.materials;
    if (a=="lines") this.state.lines = !this.state.lines;
    if (a=='points') this.state.points = !this.state.points;
    if (a=='point') this.state.point = !this.state.point;
    if (a=='volume') this.state.volume = !this.state.volume;
    if (a=='floatvolume') this.state.floatvolume = !this.state.floatvolume;
    if (a=='font') this.state.font = !this.state.font;
    if (a=='texture') this.state.texture = !this.state.texture;
    if (a=='booleanops') this.state.booleanops = !this.state.booleanops;
    if (a=='move') this.state.move = !this.state.move;
    if (a=='waveform') this.state.waveform = !this.state.waveform;
    if (a=='mainloop') this.state.mainloop = !this.state.mainloop;
    if (a=='framebuffer') this.state.framebuffer = !this.state.framebuffer;
  },
};
function striparray(e)
{
   if (e[0]=='[' && e[e.length-1]==']') {
     return e.substr(1,e.length-2);
   }
   return e;
}
function get_tooltip(e)
{
  var obj = g_objs[e];
  if (obj==null) return "err";
  var ret_type = obj[1];
  var count = obj[4];
  var i;
  var str = "(";
  var donecount = 0;
  for(i=0;i<count;i++) {
     var p_type = obj[5+i*3+1];
     var p_type2 = striparray(p_type);
     if (p_type2.length<=4 && p_type2[0]>='A' && p_type2[0]<='Z') {
       if (donecount!=0) str+=",";
       str+=p_type;
       donecount++;
     }
  }
  str+=")->" + ret_type;
  return str;
}
function fetch_one(t,ii)
{
  var arr = t.match(/\w+/g);
  var count = 0;
  array = [];
  for(i=0;i<arr.length;i++)
  {
    var elem = arr[i];
    if (elem[0]=='_') { count++; if (count==ii) array.push(elem); }
    else if (count==ii) array.push(elem);
  }
  return array;
}
function parse_all(t)
{
   //console.log(t);
   return JSON.parse(t);
}
const myHeaders = new Headers();
const myFRequest = new Request("https://meshpage.org/functions3.txt", {
  method: 'GET',
  headers: myHeaders,
  mode: 'same-origin',
  cache: 'default'
  });
var g_objs;
var g_temp_cache = { };
var g_ui_mode = 0;
var gameapi_homepageurl = "https://tpgames.org/";
var g_project_name = "empty";
fetch(myFRequest).then((r) => {
   return r.text();
   }).then((t) => {
     g_objs = parse_all(t);
   });
const myRequest = new Request("https://meshpage.org/functions.txt", {
      method: 'GET',
      headers: myHeaders,
      mode: 'same-origin',
      cache: 'default'
      });
fetch(myRequest).then((r) => { 
  return r.text();
}).then((t) => {
var app = new Vue({
  el: '#app',
    mounted: function() {
      document.addEventListener("keydown", this.ctrl_s_handler);
      //console.log("Loading...");
      var vm = this;
      retrieve_builder_data(g_project_name, vm);
    },
  methods: {
    ctrl_s_handler(e) {
    		      if (!(e.keyCode === 83 && e.ctrlKey)) return;
    		      //console.log("CTRL-S handler");
    		      var vm = this;
		      store_builder_data(g_project_name, vm);
		      if (e) { e.preventDefault(); }
		      },
    close_properties(e) { var vm = this; close_props(e,vm.properties,vm.values); },
    close_questionaire(e) { var vm = this; close_question(e,vm.questionaire); },
    select_box(e) { var vm = this;
    		      var ref = { value: vm.tempboxes };
                    selectBox(e,ref);
		    vm.tempboxes = ref.value;
		    },
    press(e) { var vm = this;
    	       var ref = { value: vm.tempboxes };
	       var ref2 = { value: vm.templines };
    	       mouseDown(e,vm.boxes,vm.popups,vm.popuptexts,vm.popuphighlight,ref,vm.values,ref2,vm.links,vm.properties, vm.questionaire);
	       vm.templines = ref2.value;
	       },
    release(e) { mouseUp(e); },
    mousemove(e) {
      	var vm = this; mouseMove(e,vm.popups,vm.popuphighlight);
    },
    close_display_dialog(e) { close_display(); },
    menumousemove(e) {
       var vm = this; menuMouseMove(e);
    },
    editchanged(e) {
      //console.log("EditChanged");
    }
  },
  data: {
  	bitmapdata: fetch_one(t,1),
	polygondata: fetch_one(t,2),
	boolbitmapdata: fetch_one(t,3),
	floatbitmapdata: fetch_one(t,4),
	materialsdata: fetch_one(t,5),
	linesdata: fetch_one(t,6),
	pointsdata: fetch_one(t,7),
	pointdata: fetch_one(t,8),
	volumedata: fetch_one(t,9),
	floatvolumedata: fetch_one(t,10),
	fontdata: fetch_one(t,11),
	texturedata: fetch_one(t,12),
	booleanopsdata: fetch_one(t,13),
	movedata: fetch_one(t,14),
	waveformdata: fetch_one(t,15),
	mainloopdata: fetch_one(t,16),
	framebufferdata: fetch_one(t,17),
	state: store.state,
	boxes: [], //{id:"box1",x:100,y:100,label:"bm_fullscreen",params:["ML","BM"],ret_value:"BM"}, {id:"box2",x:350,y:100,label:"bm_fullscreen",params:["ML","BM"],ret_value:"BM"}],
	links: [], //[{start:"box1",end:"box2",endnum:"1"}],
	values: [], // {id:"box1",params:["box2","box2"], types:["BM","BM"],func:"flip_y", api:apiname, ret:"BM"}
	tempboxes: { },
	popups: [],
	popuphighlight: [],
	popuptexts: [],
	templines: { }, // start:"box1", x2:100, y2:100
	properties: [], // {id:"box1", num: param_num, name: "foobar", type: "int", value: "0"}
	questionaire: [], // [{name:"Homepage address:", value:"http://tpgames.org/", alternatives: ["a", "b", "c"]}]
	},
  directives: {
  	templine: function(canvas,binding,vnode) {
           if (binding.value==null) return;
           var start = binding.value.start;
	   var x2 = binding.value.x2;
	   var y2 = binding.value.y2;

	  var s = vnode.context.boxes.length;
	  var j;
	  for(j=0;j<s;j++) {
             var id = vnode.context.boxes[j].id;
	             //console.log(id);
        	     //console.log(start);
	     if (id==start) break;
	  }
	  var start_box = vnode.context.boxes[j];
	  if (start_box==undefined) return;

	  var x1 = start_box.x;
	  var y1 = start_box.y;
	  var l1 = start_box.label;
	  var p1 = start_box.params;
	  var r1 = start_box.ret_value;
	  
         draw_line2(canvas,
		   x1,y1,l1,p1,r1,
		   x2,y2);

	   },
	tempbox: function(canvas,binding,vnode) {
     	  var id = binding.value.id;
	  if (id==null) return;
	  var x = binding.value.x;
	  var y = binding.value.y;
	  var label = binding.value.label;
	  var params = binding.value.params;
	  var ret_value = binding.value.ret_value;
     	  draw_box(canvas,x,y,label, params, ret_value);

	  },
     popup: function(canvas,binding,vnode) {
       var len = binding.value.length;
       var i;
       for(i=0;i<len;i++) {
       	   var m = binding.value[i];
	   var x = m[0];
	   var y = m[1];
	   draw_popup(canvas,x,y);
       }
     },
     popuphighlight: function(canvas,binding,vnode) {
        draw_popup_highlight(canvas,popup_x,popup_y,binding.value);
     },
     popuptext: function(canvas,binding,vnode) {
        draw_popup_texts(canvas,popup_x,popup_y,binding.value);
     },
     link: function(canvas,binding,vnode) {
        //console.log(canvas);
	//console.log(binding);
	//console.log(vnode);
       var len = binding.value.length;
       var i;
       for(i=0;i<len;i++) {
          var m = binding.value[i];
	  //console.log(m);
	  var start = m.start;
	  var end = m.end;
	  var num = m.endnum;
	  var s = vnode.context.boxes.length;
	  var j;
	  for(j=0;j<s;j++) {
             var id = vnode.context.boxes[j].id;
	             //console.log(id);
        	     //console.log(start);
	     if (id==start) break;
	  }
	  var start_box = vnode.context.boxes[j];
	  var s2 = vnode.context.boxes.length;
	  var k;
	  for(k=0;k<s2;k++) {
	    var id = vnode.context.boxes[k].id;
	             //console.log(id);
        	     //console.log(end);
	    if (id==end) break;
	  }
	  var end_box = vnode.context.boxes[k];

	  var x1 = start_box.x;
	  var y1 = start_box.y;
	  var l1 = start_box.label;
	  var p1 = start_box.params;
	  var r1 = start_box.ret_value;

	  var x2 = end_box.x;
	  var y2 = end_box.y;
	  var l2 = end_box.label;
	  var p2 = end_box.params;
	  var r2 = end_box.ret_value;
	 
         draw_line(canvas,
		   x1,y1,l1,p1,r1,
		   x2,y2,l2,p2,r2,num);

	  }
     },
     box: function(canvas, binding) {

	    var ctx = canvas.getContext("2d");
	    
	    ctx.clearRect(canvas.scrollLeft,canvas.scrollTop,canvas.clientWidth,canvas.clientHeight);

//console.log(binding.value);
       var m = binding.value;
       var s = m.length;
       //console.log(s);
       var i;
       for(i=0;i<s;i++) {
       	   var k = m[i];
     	   draw_box(canvas,k.x,k.y,k.label, k.params, k.ret_value);
	   
       }
       }
  }
})
});
</script>

<style>
html, body { height: 100%; margin: 0px; }
html { overflow-y: hidden; }
.app {
  height: 100%;
}
.noselect {
 -webkit-touch-callout: none; /* iOS Safari */
    -webkit-user-select: none; /* Safari */
     -khtml-user-select: none; /* Konqueror HTML */
       -moz-user-select: none; /* Firefox */
        -ms-user-select: none; /* Internet Explorer/Edge */
            user-select: none; /* Non-prefixed version, currently
                                  supported by Chrome and Opera */
}
.space {
  height: 300px;
}
.column {
  float: left;
}
.column.side {
  width: 150px;
}
.column.middle {
}
.row:after {
  content: "";
  display: table;
  clear: both;
}
.column {
  height: 100%;
  
}
.focus {
  background-color: darkred;
  border-width: 5px;
  border-style: outset;
  border-color: black;

  font-size: 20px;
  color: lightgray;
  width: 200;
  margin: 0px 0px 0px 0px;
  border-width: 5px;
  border-style: solid;
  border-color: darkred;
  text-overflow: ellipsis;

}
.scrollarea {
  height: 100%;	    
  width: 100%;
  overflow-y: scroll;
  overflow-x: hidden;
  background-color: #284848;
}
.label {
  font-size: 20px;
  color: lightgray;
  background-color: #182828;
  width: 200;
  margin: 0px 0px 0px 0px;
  border-width: 5px;
  border-style: solid;
  border-color: #182828;
  text-overflow: ellipsis;
}
.highlight:focus { outline: unset; outline-color: black; outline-width: 0px; }
.highlight {
  font-size: 25px;
  color: lightgray;
  background-color: darkslategray;
  width: 200;
  margin: 0px 0px 0px 0px;
  border-width: 5px;
  border-style: solid;
  border-color: darkslategray;
  text-overflow: ellipsis;
  text-align: left;
  padding: 0px 0px;
}
:hover.highlight {
  background-color: black;
  border-width: 5px;
  border-style: solid;
  border-color: black;
}
button::-moz-focus-inner { border:0; }
.label:focus { outline: unset; outline-color: black; outline-width: 0px; }
:hover.label {
  background-color: black;
  border-width: 5px;
  border-style: solid;
  border-color: black;
}
.canvas {
  position: absolute;
  left: 150px;
  top: 0px;
  border: 1px solid #000000;
  overflow: scroll;
  background-color: dimgray;
  backface-visibility: hidden;
}

body {
  background-color: dimgray;
}
.display {
  position: fixed;
  left: 250px;
  top: 50px;
  min-width: 100px;
  min-height: 100px;
}
.ems {
  background-color: black;
}
.table {
  position: absolute;
  left: 250px;
  top: 50px;
  display: table;
  width: auto;
  background-color: slategray;
  border-color: slategray;
  border-style: outset;
  padding: 40px 40px 10px 40px;
}
.tablerow {
  display: table-row;
  width: auto;
}
.tablecolumn {
   font-size: 45px;
   display: table-cell;
   text-align: right;
   padding: 5px 5px 5px 5px;
}
.tablecolumn2 {
   display: table-cell;
}
.edit {
      display: inline;
      font-size: 45px;
      text-align: left;
      width: 400px;
      left: 50%;
      }
.input {
   font-size: 45px;
   text-align: left;
   width: 400px;
}
.close {
  position: fixed;
  font-size: 30px;
}
.close2 {
  font-size: 30px;
  margin: 30px 0px 0px 300px;
}
.tooltip {
  position: absolute;
  top: 0px;
  left: 0px;
  border-style: outset;
  background-color: slategray;
  border-color: slategray;
  font-weight: bold;
}
</style>
<script>
function draw_line(canvas, x1,y1,label1,params1,ret_type1,
			  x2,y2,label2,params2,ret_type2,ii)
{
       var ctx = canvas.getContext("2d");

       var left_pos = get_right_box_position(canvas,x1,y1,label1,params1,ret_type1);
	    var right_pos = get_left_box_position(canvas,x2,y2,label2,params2,ret_type2,ii);
	    ctx.beginPath();
	    ctx.lineWidth = 3;
	    ctx.moveTo(left_pos[0],left_pos[1]);
	    ctx.lineTo(right_pos[0],right_pos[1]);
	    ctx.stroke();
}
function draw_line2(canvas,x1,y1,label1,params1,ret_type1,
		           x2,y2)
			   {
	    var ctx = canvas.getContext("2d");
	    var left_pos = get_right_box_position(canvas,x1,y1,label1,params1,ret_type1);
	    ctx.beginPath();
	    ctx.lineWidth = 3;
	    ctx.moveTo(Math.floor(left_pos[0]),Math.floor(left_pos[1]));
	    ctx.lineTo(Math.floor(x2),Math.floor(y2));
	    ctx.stroke();
	    
}
function draw_box(canvas,x,y,label, params, ret_type)
{
	    var ctx = canvas.getContext("2d");

	    ctx.font = "20px Verdana";
	    // box dimensions
	    var wd;
	    if (typeof g_temp_cache[label] !== "undefined") wd=g_temp_cache[label];
	     else {
		wd = ctx.measureText(label).width;
		g_temp_cache[label]=wd;
		}

	    //console.log(wd);
	    var ht = 20;
	    var mg = 10;
	    var bw = mg+wd+mg;
	    if (bw<100) bw=100;
	    var hh = mg+ht+mg;
	    // slot dimensions
	    ctx.font = "16px Verdana";
	    var s = params.length;
	    var s_wd = 0;
	    var delta = [];
	    for(i=0;i<s;i++) {
              var wdd;
	      if (typeof g_temp_cache[params[i]] !== "undefined") wdd=g_temp_cache[params[i]];
	      else {
	        wdd = ctx.measureText(params[i]).width;
		g_temp_cache[params[i]] = wdd;
		}
	    //console.log(wdd);
	      if (wdd>s_wd) s_wd=wdd;
	    }
	    for(i=0;i<s;i++) {
              var wdd;
	      if (typeof g_temp_cache[params[i]] !== "undefined") wdd=g_temp_cache[params[i]];
	      else {
	        wdd = ctx.measureText(params[i]).width;
		g_temp_cache[params[i]] = wdd;
		}
	    //console.log(wdd);
	      delta.push(s_wd-wdd);
	      }
              var wdd2;
	      if (typeof g_temp_cache[ret_type] !== "undefined") wdd2=g_temp_cache[ret_type];
	      else {
	        wdd2 = ctx.measureText(ret_type).width;
		g_temp_cache[ret_type] = wdd2;
		}
	    //console.log(wdd2);
	      if (wdd2>s_wd) s_wd=wdd2;
              var wx;
	      if (typeof g_temp_cache[ret_type] !== "undefined") wx=g_temp_cache[ret_type];
	      else {
	        wx = ctx.measureText(ret_type).width;
		g_temp_cache[ret_type] = wx;
		}
	    //console.log(wx);
	    var r_delta = s_wd-wx;
	    
	    var s_ht = 12; // from font height
	    var s_mg = 6; // margin
	    var s_bw = 2 + s_mg + s_wd + s_mg + 2;
	    var s_hh = s_mg + s_ht + s_mg;

	    var hg = 2+hh + mg + s*(s_hh+mg) +2;
	    if (hg < 120) hg=120;

	    var s_gmg = (hg-4-hh - mg - s*(s_hh+mg))/2;

	    var r_pos = hh + (hg-hh)/2;


	    // box borders
	    ctx.fillStyle = "#000000";
	    ctx.fillRect(Math.floor(x),Math.floor(y),Math.floor(bw),Math.floor(hg));
	    ctx.fillStyle = "slategray";
	    ctx.fillRect(Math.floor(x+2),Math.floor(y+2),Math.floor(bw-4),Math.floor(hg-4));
	    // box header	    
	    ctx.fillStyle = "#000000";
	    ctx.fillRect(Math.floor(x),Math.floor(y),Math.floor(bw),Math.floor(hh));
	    // text
	    ctx.font = "20px Verdana";
	    ctx.fillStyle = "lightgray";
	    ctx.fillText(label, Math.floor(x+mg), Math.floor(y+mg+ht));
	    // left side slots
	    for(i=0;i<s;i++) {
	      var s_label = params[i];
	      // slot
	      ctx.fillStyle = "#000000";
	      ctx.fillRect(Math.floor(x), Math.floor(y+hh+mg+i*(s_hh+mg)+s_gmg), Math.floor(s_bw), Math.floor(s_hh));
	      ctx.fillStyle = "darkslategray";
	      ctx.fillRect(Math.floor(x+2),Math.floor(y+hh+mg+i*(s_hh+mg)+2+s_gmg), Math.floor(s_bw-4), Math.floor(s_hh-4));
	      // slot text
	      ctx.font = "16px Verdana";
	      ctx.fillStyle = "lightgray";
	      ctx.fillText(s_label, Math.floor(x+s_mg/2+delta[i]/2), Math.floor(y+hh+mg+i*(s_hh+mg) + s_mg + s_ht + s_gmg));
	      }
	   // right side slot
	   ctx.fillStyle = "#000000";
	   ctx.fillRect(Math.floor(x + bw - s_bw), Math.floor(y+r_pos - s_hh/2), Math.floor(s_bw), Math.floor(s_hh));
	   ctx.fillStyle = "darkslategray";
	   ctx.fillRect(Math.floor(x+bw+2-s_bw), Math.floor(y+r_pos-s_hh/2+2), Math.floor(s_bw-4), Math.floor(s_hh-4));
	   // right side text
	   ctx.font = "16px Verdana";
	   ctx.fillStyle = "lightgray";
	   ctx.fillText(ret_type, Math.floor(x + bw-s_bw +s_mg/2+r_delta/2), Math.floor(y+r_pos - (s_hh-4)/2 -2 + s_mg + s_ht) );
}
function get_box_title_location(canvas,x,y,label,params,ret_type)
{
	    var ctx = canvas.getContext("2d");

	    ctx.font = "20px Verdana";
	    // box dimensions
	    var wd;
	    if (typeof g_temp_cache[label] !== "undefined") wd=g_temp_cache[label];
	     else {
		wd = ctx.measureText(label).width;
		g_temp_cache[label]=wd;
		}
	    var ht = 20;
	    var mg = 10;
	    var bw = mg+wd+mg;
	    if (bw<100) bw=100;
	    var hh = mg+ht+mg;
	    return [x,y,bw,hh];
}
function get_box_location(canvas,x,y,label,params,ret_type)
{
	    var ctx = canvas.getContext("2d");

	    ctx.font = "20px Verdana";
	    // box dimensions

	    var wd;
	    if (typeof g_temp_cache[label] !== "undefined") wd=g_temp_cache[label];
	     else {
		wd = ctx.measureText(label).width;
		g_temp_cache[label]=wd;
		}
	    
	    var ht = 20;
	    var mg = 10;
	    var bw = mg+wd+mg;
	    if (bw<100) bw=100;
	    var hh = mg+ht+mg;
	    // slot dimensions
	    ctx.font = "16px Verdana";
	    var s = params.length;
	    var s_wd = 0;
	    var delta = [];
	    for(i=0;i<s;i++) {
	                  var wdd;
	      if (typeof g_temp_cache[params[i]] !== "undefined") wdd=g_temp_cache[params[i]];
	      else {
	        wdd = ctx.measureText(params[i]).width;
		g_temp_cache[params[i]] = wdd;
		}
	      if (wdd>s_wd) s_wd=wdd;
	    }
	    for(i=0;i<s;i++) {
	                  var wdd;
	      if (typeof g_temp_cache[params[i]] !== "undefined") wdd=g_temp_cache[params[i]];
	      else {
	        wdd = ctx.measureText(params[i]).width;
		g_temp_cache[params[i]] = wdd;
		}
	      delta.push(s_wd-wdd);
	      }
              var wdd2;
	      if (typeof g_temp_cache[ret_type] !== "undefined") wdd2=g_temp_cache[ret_type];
	      else {
	        wdd2 = ctx.measureText(ret_type).width;
		g_temp_cache[ret_type] = wdd2;
		}	      
	      if (wdd2>s_wd) s_wd=wdd2;

              var wx;
	      if (typeof g_temp_cache[ret_type] !== "undefined") wx=g_temp_cache[ret_type];
	      else {
	        wx = ctx.measureText(ret_type).width;
		g_temp_cache[ret_type] = wx;
		}
	    var r_delta = s_wd-wx;
	    
	    var s_ht = 12; // from font height
	    var s_mg = 6; // margin
	    var s_bw = 2 + s_mg + s_wd + s_mg + 2;
	    var s_hh = s_mg + s_ht + s_mg;

	    var hg = 2+hh + mg + s*(s_hh+mg) +2;
	    if (hg < 120) hg=120;

	    var s_gmg = (hg-4-hh - mg - s*(s_hh+mg))/2;

	    var r_pos = hh + (hg-hh)/2;
	    return [x,y,bw,hg];
}
function get_left_box_position(canvas,x,y,label, params, ret_type, ii)
{
	    var ctx = canvas.getContext("2d");

	    ctx.font = "20px Verdana";
	    // box dimensions
	    	    var wd;
	    if (typeof g_temp_cache[label] !== "undefined") wd=g_temp_cache[label];
	     else {
		wd = ctx.measureText(label).width;
		g_temp_cache[label]=wd;
		}
	    var ht = 20;
	    var mg = 10;
	    var bw = mg+wd+mg;
	    if (bw<100) bw=100;
	    var hh = mg+ht+mg;
	    // slot dimensions
	    ctx.font = "16px Verdana";
	    var s = params.length;
	    var s_wd = 0;
	    var delta = [];
	    for(i=0;i<s;i++) {
	    	                  var wdd;
	      if (typeof g_temp_cache[params[i]] !== "undefined") wdd=g_temp_cache[params[i]];
	      else {
	        wdd = ctx.measureText(params[i]).width;
		g_temp_cache[params[i]] = wdd;
		}
	      if (wdd>s_wd) s_wd=wdd;
	    }
	    for(i=0;i<s;i++) {
	                  var wdd;
	      if (typeof g_temp_cache[params[i]] !== "undefined") wdd=g_temp_cache[params[i]];
	      else {
	        wdd = ctx.measureText(params[i]).width;
		g_temp_cache[params[i]] = wdd;
		}
	      delta.push(s_wd-wdd);
	      }
              var wdd2;
	      if (typeof g_temp_cache[ret_type] !== "undefined") wdd2=g_temp_cache[ret_type];
	      else {
	        wdd2 = ctx.measureText(ret_type).width;
		g_temp_cache[ret_type] = wdd2;
		}
	      if (wdd2>s_wd) s_wd=wdd2;

              var wx;
	      if (typeof g_temp_cache[ret_type] !== "undefined") wx=g_temp_cache[ret_type];
	      else {
	        wx = ctx.measureText(ret_type).width;
		g_temp_cache[ret_type] = wx;
		}
	    var r_delta = s_wd-wx;
	    
	    var s_ht = 12; // from font height
	    var s_mg = 6; // margin
	    var s_bw = 2 + s_mg + s_wd + s_mg + 2;
	    var s_hh = s_mg + s_ht + s_mg;

	    var hg = 2+hh + mg + s*(s_hh+mg) +2;
	    if (hg < 120) hg=120;

	    var s_gmg = (hg-4-hh - mg - s*(s_hh+mg))/2;

	    var r_pos = hh + (hg-hh)/2;
	    var xx = x;
	    var yy = y+hh+mg+ii*(s_hh+mg)+s_gmg + s_hh/2;
	    return [xx,yy,s_bw,s_hh];

}
function get_right_box_position(canvas, x,y,label,params,ret_type)
{
	    var ctx = canvas.getContext("2d");

	    ctx.font = "20px Verdana";
	    // box dimensions
	    	    var wd;
	    if (typeof g_temp_cache[label] !== "undefined") wd=g_temp_cache[label];
	     else {
		wd = ctx.measureText(label).width;
		g_temp_cache[label]=wd;
		}

	    var ht = 20;
	    var mg = 10;
	    var bw = mg+wd+mg;
	    if (bw<100) bw=100;
	    var hh = mg+ht+mg;
	    // slot dimensions
	    ctx.font = "16px Verdana";
	    var s = params.length;
	    var s_wd = 0;
	    var delta = [];
	    for(i=0;i<s;i++) {
	                  var wdd;
	      if (typeof g_temp_cache[params[i]] !== "undefined") wdd=g_temp_cache[params[i]];
	      else {
	        wdd = ctx.measureText(params[i]).width;
		g_temp_cache[params[i]] = wdd;
		}
	      if (wdd>s_wd) s_wd=wdd;
	    }
	    for(i=0;i<s;i++) {
	                  var wdd;
	      if (typeof g_temp_cache[params[i]] !== "undefined") wdd=g_temp_cache[params[i]];
	      else {
	        wdd = ctx.measureText(params[i]).width;
		g_temp_cache[params[i]] = wdd;
		}
	      delta.push(s_wd-wdd);
	      }
	      var wdd2 = ctx.measureText(ret_type).width;
	      if (wdd2>s_wd) s_wd=wdd2;

              var wx;
	      if (typeof g_temp_cache[ret_type] !== "undefined") wx=g_temp_cache[ret_type];
	      else {
	        wx = ctx.measureText(ret_type).width;
		g_temp_cache[ret_type] = wx;
		}
	    var r_delta = s_wd-wx;
	    
	    var s_ht = 12; // from font height
	    var s_mg = 6; // margin
	    var s_bw = 2 + s_mg + s_wd + s_mg + 2;
	    var s_hh = s_mg + s_ht + s_mg;

	    var hg = 2+hh + mg + s*(s_hh+mg) +2;
	    if (hg < 120) hg=120;

	    var s_gmg = (hg-4-hh - mg - s*(s_hh+mg))/2;

	    var r_pos = hh + (hg-hh)/2;

	   // right side slot
	   var xx = x+bw;
	   var yy = y+r_pos-s_hh/2+s_hh/2;
	   return [xx,yy,s_bw,s_hh];
}
var grabbed_object = null;
var grab_dx = 0;
var grab_dy = 0;

var created_object = null;
var created_value = null;

var grabbed_line = null;

function getMousePos(canvas, evt) {
  var rect = canvas.getBoundingClientRect();
  return { x: evt.clientX - rect.left,
           y: evt.clientY - rect.top
	   };
	 }
var timeout = null;
function menuMouseMove(e)
{
   var trg = e.target;
   var bounds = trg.getBoundingClientRect();
   var hml = trg.innerHTML;
   var canvas = document.getElementById('myCanvas');
   var pos = getMousePos(canvas,e);
   var tooltip = document.getElementById('tooltip');
   tooltip.innerHTML = get_tooltip(hml);
   tooltip.style.left = "120px";
   tooltip.style.top = bounds.top.toString() + "px";
   tooltip.style.display="block";

   if (timeout)
     window.clearTimeout(timeout);
   timeout = window.setTimeout(function() { tooltip.style.display="none"; },1500);

}
function mouseMove(e,popups,popuphighlight)
{
//   console.log(pos);
   if (grabbed_object) {
   var canvas = document.getElementById('myCanvas');
   var pos = getMousePos(canvas,e);
     grabbed_object.x = pos.x-grab_dx;
     grabbed_object.y = pos.y-grab_dy;
   }
   if (created_object) {
   var canvas = document.getElementById('myCanvas');
   var pos = getMousePos(canvas,e);
      created_object.value.x = pos.x;
      created_object.value.y = pos.y;
   }
   if (grabbed_line) {
   var canvas = document.getElementById('myCanvas');
   var pos = getMousePos(canvas,e);
      grabbed_line.value.x2 = pos.x;
      grabbed_line.value.y2 = pos.y;
   }

   if (popups.length>0) {
   var canvas = document.getElementById('myCanvas');
   var pos = getMousePos(canvas,e);
   var popup_x = popups[0][0];
   var popup_y = popups[0][1];
   if (pos.x>popup_x && pos.y>popup_y && pos.x<popup_x+popup_width && pos.y<popup_y+popup_height)
   {
       var x = pos.x-popup_x;
       var y = pos.y-popup_y;
       while(popuphighlight.length>0) popuphighlight.pop();
       popuphighlight.push(x);
       popuphighlight.push(y);
	// inside popup
   } else
   {
       while(popuphighlight.length>0) popuphighlight.pop();
   }
   

   }


}

function set_keyboard_focus()
{
  var iframe = document.getElementById('ems');
  iframe.contentWindow.focus();
}


var g_key_timeout = null;
function set_keyboard_focus_to_iframe()
{
  if (g_key_timeout)
     window.clearTimeout(g_key_timeout);

  g_key_timeout = setTimeout(set_keyboard_focus,100);
}

function is_array(type)
{
  if (type.length>2 && type[0]=='[' && type[type.length-1]==']') return true;
  return false;
}
function value_to_array(value)
{
  //console.log(value);
  var val2 = value.substr(1,value.length-2);
  if (val2=="") return [];
  var words = val2.split(',');
  //console.log(words);
  return words;
}
function array_to_value(words)
{
  var s = words.length;
  var i;
  var res = "[";
  for(i=0;i<s;i++)
  {
    res+=words[i];
    if (i!=s-1) res+=",";
  }
  res+="]";
  return res;
}
var g_resize_timeout = null;
function check_for_resize()
{
     var closebutton = document.getElementById("close");
     var iframe = document.getElementById("ems");
     var win = window.frames["ems"].contentWindow;
     var cnv = win.document.getElementById("canvas");

     if (closebutton != null && iframe != null && cnv != null) {
       var elem = document.getElementById("display");
	elem.style.display = "block";
     	resize_event(null);
	win.addEventListener('keydown', (e) => {
	   //console.log(e);
	   if (e.key == 'Escape') {
	     close_display();
	   }
	   });


	return;
	}

 if (g_resize_timeout)
   window.clearTimeout(g_resize_timeout);
 g_resize_timeout = window.setTimeout(check_for_resize,100);
}

var g_display_timeout = null;

function display_cb()
{
   var elem = document.getElementById("display");
   elem.style.display = "block";
   g_display_timeout = null;
}

var g_emscripten_running = false;
var g_emscripten_start_count = 0;
var Module = null;
var g_popup_open = false;
var g_resize2_timeout = null;
var g_resize3_timeout = null;
function mouseDown(e,boxes,popups,popuptexts,popuphighlight,tempboxes,values,templines,links, properties, questionaire)
{

   if (e.button == 2) { // right mouse button
    // clear all popups
    while(popups.length>0) popups.pop();
    while(popuptexts.length>0) popuptexts.pop();


   var canvas = document.getElementById('myCanvas');
   var pos = getMousePos(canvas,e);
   var s = boxes.length;
   var i;
   for(i=0;i<s;i++) {
     var x = boxes[i].x;
     var y = boxes[i].y;
     var label = boxes[i].label;
     var params = boxes[i].params;
     var ret_type = boxes[i].ret_type;
     var loc = get_box_location(canvas,x,y,label,params,ret_type);
     if (pos.x>loc[0] && pos.x<loc[0]+loc[2] && pos.y>loc[1] && pos.y<loc[1]+loc[3]) {
        popup_menu(popups,popuptexts,pos.x,pos.y);
	g_popup_open = true;
     	return;
     }
     //console.log(x);
     //console.log(y);
   }
   }
   if (e.button == 0) { // left mouse button
   var canvas = document.getElementById('myCanvas');
   var pos = getMousePos(canvas,e);

   if (!g_popup_open) {

   var ss = boxes.length;
   var i;
   for(i=0;i<ss;i++) {
      var box = boxes[i];
      var bb = get_right_box_position(canvas,box.x,box.y,box.label,box.params,box.ret_type);
      var dx = pos.x-bb[0];
      var dy = pos.y-bb[1];
      if (dx>-bb[2]/2 && dx<0 && dy>-bb[3]/2 && dy<bb[3]/2) {
	 templines.value = { start: box.id, x2:pos.x, y2:pos.y};	 	 
         grabbed_line = templines;
	 return;
      }
   }
}

   if (grabbed_line && !g_popup_open) {
      var ss = boxes.length;
      var i;
      for(i=0;i<ss;i++) {
      	 var box = boxes[i];
	 var num = box.params.length;
	 var j;
	 for(j=0;j<num;j++) {
            var bb = get_left_box_position(canvas,box.x,box.y,box.label,box.params,box.ret_type,j);
	    var dx = pos.x-bb[0];
            var dy = pos.y-bb[1];
            if (dx>0 && dx<bb[2] && dy>-bb[3] && dy<bb[3]) {
	      var v = { start: grabbed_line.value.start, end: box.id, endnum: j};
	      var type1 = box.params[j];
	      var k;
	      var sk = boxes.length;
	      for(k=0;k<sk;k++) {
	         if (boxes[k].id==v.start) {
		    if (striparray(type1) != striparray(boxes[k].ret_value)) {
		    //console.log(type1);
		    //console.log(boxes[k].ret_value);
	            grabbed_line.value = null;
     		    grabbed_line = null;
     		    templines.value = null;
		    return;
		    }
		 }
	      }


	      links.push(v);

	      var old_val;
	      var u;
	      var uk = values.length;
	      for(u=0;u<uk;u++) {
	         var val = values[u];
		 //console.log(val.id);
		 //console.log(v.end);
		 if (val.id == v.end) {
		   var params = val.types;
		   //console.log(params);
		   var ok = params.length;
		   var o;
		   var count = 0;
		   for(o=0;o<ok;o++) {
		      var p = params[o];
		      //console.log(p);
		      if (is_array(p)) {
		        if (count==j) {
			   old_val = "@";
			   var arr = value_to_array(val.params[o]);
			   arr.push(v.start);
			   val.params[o] = array_to_value(arr);
			}
			count=count+1;
		      } else
		      if (p.length<=4 && p[0]>='A' && p[0]<='Z') {
		      	 if (count==j) {
			    old_val = val.params[o];
			    val.params[o] = v.start;
			    //console.log(val.params);
		      	 }
		         count=count+1;
		      }
		   }
		 }
	      }

	      //console.log(old_val);
	      //console.log(box.id);
	      //console.log(j);

	      var sj = links.length;
	      var uu;
	      for(uu=0;uu<sj;uu++) {
	         var l = links[uu];
		 if (l.start == old_val && l.end==box.id && l.endnum==j)
		 {
		 links.splice(uu,1);
		 break;
		 }
	      }

	      grabbed_line.value = null;
     	      grabbed_line = null;
     	      templines.value = null;
     	      return;

            }
         }

	 }
     grabbed_line.value = null;
     grabbed_line = null;
     templines.value = null;
     return;
   }


   if (created_object && !g_popup_open) {
    boxes.push(created_object.value);
    created_object.value = null;
    created_object = null;
    tempboxes.value = null;

    values.push(created_value.value);
    created_value.value = null;
    created_value = null;

    selected_item.classList.remove('focus');
    selected_item.classList.add('label');

    return;
   }


   if (popups.length>0) {
   var popup_x = popups[0][0];
   var popup_y = popups[0][1];
   if (!(pos.x>popup_x && pos.y>popup_y && pos.x<popup_x+popup_width && pos.y<popup_y+popup_height))
   {
	// outside of popup -> remove popup
	g_popup_open = false;
	popups.pop();
	while(popuptexts.length>0)
	   popuptexts.pop();
   } else {
	g_popup_open = false;

       while(popups.length>0) popups.pop();
       while(popuptexts.length>0) popuptexts.pop();
       while(popuphighlight.length>0) popuphighlight.pop();

     clicknum = click_popup(canvas,pos.x-popup_x,pos.y-popup_y);
     //console.log(clicknum);

     if (clicknum==2) // questionaire
     {
        var v = { name: "Homepage address", value: gameapi_homepageurl, alternatives: [] };
	questionaire.push(v);
	var v = { name: "Project name", value: g_project_name, alternatives: [] };
	questionaire.push(v);

        var el = document.getElementById('questionaireclose');
        el.style.display = "block";
        var el = document.getElementById('qtable');
        el.style.display = "table";
     }
     if (clicknum==0) // properties
     {
        var el = document.getElementById('propertyclose');
        el.style.display = "block";
        var el = document.getElementById('table');
        el.style.display = "table";

        var s = boxes.length;
   	var i;
   	for(i=0;i<s;i++) {
     	  var x = boxes[i].x;
          var y = boxes[i].y;
          var label = boxes[i].label;
          var params = boxes[i].params;
          var ret_type = boxes[i].ret_value;
          var loc = get_box_location(canvas,x,y,label,params,ret_type);
          if (popup_x>loc[0] && popup_x<loc[0]+loc[2] && popup_y>loc[1] && popup_y<loc[1]+loc[3]) {
	     // box #i was chosen

             while(properties.length>0) properties.pop();
	     
	     var box = boxes[i];
	     var id = box.id;
	     var kk;
	     var kk_count = values.length;
	     for(kk=0;kk<kk_count;kk++)
	     {
	        if (values[kk].id == id) break;
	     }
	     var value = values[kk];

	     var label = box.label;
	     var obj = g_objs[label];
	     //console.log(obj);
	     var pcount = obj[4];
	     var ii;
	     var count = 0;
	     for(ii=0;ii<pcount;ii++)
	     {
		var paramname = obj[5+ii*3+0];
		var paramtype = obj[5+ii*3+1];
		var val = value.params[ii];
		if ((paramtype.length<=4 && paramtype[0]>='A' && paramtype[0]<='Z')||is_array(paramtype)) {
		} else {
		  if (paramname != "ev") {
		  var v = { id: id, num: ii, name: paramname, type: paramtype, value: val };
		  properties.push(v);
		  }
		}
	     }
	  }
	}
       return;
     }

     if (clicknum==3) // display
     {
        g_ui_mode = 0;
        var str = "";
        var s = boxes.length;
   	var i;
	var final_ret_type = "";
   	for(i=0;i<s;i++) {
     	  var x = boxes[i].x;
          var y = boxes[i].y;
          var label = boxes[i].label;
          var params = boxes[i].params;
          var ret_type = boxes[i].ret_value;

	  //console.log("@" + ret_type + "@");
          var loc = get_box_location(canvas,x,y,label,params,ret_type);
          if (popup_x>loc[0] && popup_x<loc[0]+loc[2] && popup_y>loc[1] && popup_y<loc[1]+loc[3]) {
	     codegen_num = 1;
	     codegen_map = { };
	     str = codegen(boxes[i].id,values);
	     final_ret_type=ret_type;
	     //console.log(str);
	   }
	   }
	  if (final_ret_type=="BO") {
	    str+="P I999=ev.bool_api.to_polygon(I" + (codegen_num-1).toString() + ");@";
	     str+="ML I333=ev.polygon_api.render_vertex_array_ml2(ev,I999);@";
	     str+="ML I555=ev.mainloop_api.touch_rotate(ev,I333,true,true,0.01,0.01);@";
	     str+="RUN I666=ev.blocker_api.game_window2(ev,I555,false,false,0.0,100000.0);@";
	    
	  }
	  if (final_ret_type=="VA") {
	  }
	  if (final_ret_type=="VX") { 
            str+="P I812=ev.polygon_api.cube(-10,10,-10,10,-10,10);@";
	    str+="P I813=ev.polygon_api.cube(-10,10,-10,10,-10,10);@";
	    str+="P I814=ev.polygon_api.color(I813,ffff0000);@";
	    str+="P I815=ev.polygon_api.cube(-10,10,-10,10,-10,10);@";
	    str+="P I816=ev.polygon_api.color(I815,ffffff00);@";
	    str+="[PTS] I819=ev.voxel_api.voxel_instancing(I" + (codegen_num-1).toString() +",3,-300,300,-300,300,-300,300);@";
	    str+="MT I820=ev.materials_api.m_def(ev);@";
	    str+="MT I823=ev.materials_api.phong(ev,I820,-0.3,0.3,-1.0,ff888888,ff888888,150.0);@";
	    str+="MT I824=ev.materials_api.colour_material(ev,0.5);@";
	    str+="MT I825=ev.materials_api.combine_materials(ev,I824,I823);@";
	    str+="ML I821=ev.voxel_api.voxel_bind(ev,std::vector<P>{I812,I814,I816},I819,I825);@";

	     str+="ML I447=ev.mainloop_api.touch_rotate(ev,I821,true,true,0.01,0.01);@";
	    
	  str+="RUN I891=ev.blocker_api.game_window2(ev,I447,false,false,0.0,100000.0);@";
	    
	  }
	  if (final_ret_type=="FML") {
            str+="FBU I444=ev.low_frame_api.low_framebuffer(I" + (codegen_num-1).toString() +",4,800,600,0);@";
	    str+="RUN I445=ev.low_frame_api.low_framebuffer_run(ev,I444,0,800,600);@";
	  }
	  if (final_ret_type=="ML") {
	  str+="ML I444=ev.mainloop_api.touch_rotate(ev,I" + (codegen_num-1).toString() + ",true,true,0.01,0.01);@";

	  str+="RUN I891=ev.blocker_api.game_window2(ev,I444,false,false,0.0,100000.0);@";

	  }
	  if (final_ret_type=="MT") {
	    str+="PT I444=ev.point_api.point(0.0,0.0,0.0);@";
	    str+="P I445=ev.polygon_api.sphere(I444,350,30,30);@";
	    str+="ML I446=ev.materials_api.bind(I445,I" + (codegen_num-1).toString() + ");@";
	     str+="ML I447=ev.mainloop_api.touch_rotate(ev,I446,true,true,0.01,0.01);@";
	    
	  str+="RUN I891=ev.blocker_api.game_window2(ev,I447,false,false,0.0,100000.0);@";
	   

	  }
	  if (final_ret_type=="KF") {
	     str+="ML I443=ev.vertex_anim_api.vertex_anim_render(ev,I" + (codegen_num-1).toString() + ");@";
	  str+="ML I444=ev.mainloop_api.touch_rotate(ev,I443,true,true,0.01,0.01);@";

	  str+="RUN I891=ev.blocker_api.game_window2(ev,I444,false,false,0.0,100000.0);@";

	  }
	  if (final_ret_type=="BLK") {
	     // nothing to do
	  }
	  if (final_ret_type=="RUN") {
	     g_ui_mode = 1;
	     // nothing to do
	  }
	  if (final_ret_type=="O") {
	     str+="P I332=ev.volume_api.rendercubes3(I" + (codegen_num-1).toString() + ", 255,255,255,-300.0,300.0,-300.0,300.0,-300.0,300.0);@";
	     
	     str+="ML I333=ev.polygon_api.render_vertex_array_ml2(ev,I332);@";
	     str+="ML I444=ev.mainloop_api.touch_rotate(ev,I333,true,true,0.01,0.01);@";
	     str+="RUN I555=ev.blocker_api.game_window2(ev,I444,false,false,0.0,100000.0);@";

	  }
	  if (final_ret_type=="IBM") {
	     str+="BM I332=ev.bitmap_api.intbitmap_bm(I" + (codegen_num-1).toString() +");@";
	    str+="BM I892=ev.bitmap_api.flip_y(I332);@";
	    str+="BM I894=ev.bitmap_api.scale_to_size(I892,900);@";
	    str+="P I888=ev.polygon_api.bitmapsized_quad(I894);@"

	    str+="MT I889=ev.materials_api.texture(ev,I892,1.0);@"
	    str+="ML I890=ev.materials_api.bind(I888,I889);@"
	    str+="RUN I891=ev.blocker_api.game_window2(ev,I890,false,false,0.0,100000.0);@";
	     
	  }
	  if (final_ret_type=="BM") {
	    str+="BM I892=ev.bitmap_api.flip_y(I" + (codegen_num-1).toString() + ");@";
	    str+="BM I894=ev.bitmap_api.scale_to_size(I892,900);@";
	    str+="P I888=ev.polygon_api.bitmapsized_quad(I894);@"

	    str+="MT I889=ev.materials_api.texture(ev,I892,1.0);@"
	    str+="ML I890=ev.materials_api.bind(I888,I889);@"
	    str+="RUN I891=ev.blocker_api.game_window2(ev,I890,false,false,0.0,100000.0);@";

	  }
	  if (final_ret_type=="WV") {
	     str+="BM I332=ev.waveform_api.waveform_bitmap(I" + (codegen_num-1).toString() + ",300,100,000000,ffffffff);@";
	    str+="BM I892=ev.bitmap_api.flip_y(I332);@";
	    str+="BM I894=ev.bitmap_api.scale_to_size(I892,900);@";
	    str+="P I888=ev.polygon_api.bitmapsized_quad(I894);@"

	    str+="MT I889=ev.materials_api.texture(ev,I892,1.0);@"
	    str+="ML I890=ev.materials_api.bind(I888,I889);@"
	    str+="RUN I891=ev.blocker_api.game_window2(ev,I890,false,false,0.0,100000.0);@";
	     
	  }
	  if (final_ret_type=="CBM") {
	     str+="BM I332=ev.cont_bitmap_api.sample(I" + (codegen_num-1).toString() +",200,200);@";
	    str+="BM I892=ev.bitmap_api.flip_y(I332);@";
	    str+="BM I894=ev.bitmap_api.scale_to_size(I892,900);@";
	    str+="P I888=ev.polygon_api.bitmapsized_quad(I894);@"

	    str+="MT I889=ev.materials_api.texture(ev,I892,1.0);@"
	    str+="ML I890=ev.materials_api.bind(I888,I889);@"
	    str+="RUN I891=ev.blocker_api.game_window2(ev,I890,false,false,0.0,100000.0);@";
	     
	  }
	  if (final_ret_type=="BB") {
	    str+="BM I893=ev.bool_bitmap_api.to_bitmap(I" + (codegen_num-1).toString() +",255,255,255,255,0,0,0,0);@";
	    str+="BM I892=ev.bitmap_api.flip_y(I893);@";
	    str+="BM I894=ev.bitmap_api.scale_to_size(I892,900);@";
	    str+="P I888=ev.polygon_api.bitmapsized_quad(I894);@"
	    
	    str+="MT I889=ev.materials_api.texture(ev,I892,1.0);@";
	    str+="ML I890=ev.materials_api.bind(I888,I889);@";
	    str+="RUN I891=ev.blocker_api.game_window2(ev,I890,false,false,0.0,100000.0);@";

	  }
	  if (final_ret_type=="FB") {
	    str+="BM I893=ev.float_bitmap_api.to_grayscale_color(I" + (codegen_num-1).toString() +",255,255,255,255,0,0,0,0);@";
	    str+="BM I892=ev.bitmap_api.flip_y(I893);@";
	    str+="BM I894=ev.bitmap_api.scale_to_size(I892,900);@";
	    str+="P I888=ev.polygon_api.bitmapsized_quad(I894);@"
	    
	    str+="MT I889=ev.materials_api.texture(ev,I892,1.0);@";
	    str+="ML I890=ev.materials_api.bind(I888,I889);@";
	    str+="RUN I891=ev.blocker_api.game_window2(ev,I890,false,false,0.0,100000.0);@";
      
	  }
	  if (final_ret_type=="P") {
	     str+="ML I333=ev.polygon_api.render_vertex_array_ml2(ev,I" + (codegen_num-1).toString() + ");@";
	     str+="ML I444=ev.mainloop_api.touch_rotate(ev,I333,true,true,0.01,0.01);@";
	     str+="RUN I555=ev.blocker_api.game_window2(ev,I444,false,false,0.0,100000.0);@";

	  }
	  if (final_ret_type=="MN") {
	  }
	  if (final_ret_type=="LI") {
	  str+="LA I811=ev.lines_api.prepare(I" + (codegen_num-1).toString() +");@";
	  str+="ML I812=ev.lines_api.render_ml(ev,I811,1.0);@";
	  str+="ML I813=ev.mainloop_api.touch_rotate(ev,I812,true,true,0.01,0.01);@";
	  str+="RUN I555=ev.blocker_api.game_window2(ev,I813,false,false,0.0,100000.0);@";
	  

	  }
	  if (final_ret_type=="C") {	
	     str+="LI I312=ev.curve_api.to_lines(I" + (codegen_num-1).toString() + ",40);@";
	     
	  str+="LA I811=ev.lines_api.prepare(I312);@";
	  str+="ML I812=ev.lines_api.render_ml(ev,I811,1.0);@";
	  str+="ML I813=ev.mainloop_api.touch_rotate(ev,I812,true,true,0.01,0.01);@";
	  str+="RUN I555=ev.blocker_api.game_window2(ev,I813,false,false,0.0,100000.0);@";
	  }
	  if (final_ret_type=="PTS") {


	  str+="PT I700=ev.point_api.point(0.0,0.0,0.0);@";
	  str+="P I701=ev.polygon_api.sphere(I700,5.0,5,5);@";
	  str+="MT I705=ev.materials_api.m_def(ev);@";
	  str+="MT I706=ev.materials_api.phong(ev,I705,-0.3,0.3,-1.0,ffff8800,ff666666,5.0);@";
	  str+="ML I707=ev.materials_api.bind_inst(I701,I" + (codegen_num-1).toString() + ",I706);@";

	  str+="ML I313=ev.mainloop_api.touch_rotate(ev,I707,true,true,0.01,0.01);@";
	  str+="RUN I555=ev.blocker_api.game_window2(ev,I313,false,false,0.0,100000.0);@";
	  

	  }


	g_emscripten_start_count++;
	if (g_emscripten_running && g_emscripten_start_count<10) {
	   Module = window.frames["ems"].contentWindow.Module;
	   //console.log(Module);
	   str = str.replace(/@/g,"\n");
	   try {
	   Module.ccall('set_string', null, ['number', 'string'],[0,str]);
	   } catch(e) { }
	   //console.log(str);
	   if (g_display_timeout)
	      window.clearTimeout(g_display_timeout);
	   g_display_timeout = window.setTimeout(display_cb,2);

	   set_keyboard_focus_to_iframe();
	   if (g_resize2_timeout) {
	      window.clearTimeout(g_resize2_timeout);
	   }
	   g_resize2_timeout = window.setTimeout(function() { resize_event(null); },10);


	   return;
	} else {
	  g_emscripten_start_count = 0;
	var elem = document.getElementById("display");
   	var html = "";
	var sz = resize_size();
	html+="<button id=\"close\" class=\"close\" onclick=\"close_display(0)\">Close</button>";
	html+="<iframe class=\"ems\" id=\"ems\" width=\"" + sz[0].toString() + "\" height=\"" + sz[1].toString() +"\" src=\"https://meshpage.org/builder_display.php?code=";
	html+=str;
	html+="&homepage=" + gameapi_homepageurl;
	html+="\"></iframe>";
	elem.innerHTML = html;
	if (g_resize3_timeout)
	   window.clearTimeout(g_resize3_timeout);
	g_resize3_timeout = window.setTimeout(check_for_resize,100);

	g_emscripten_running = true;
	set_keyboard_focus_to_iframe();

	return;
	}
     }
     if (clicknum==1) // codegen
     {

        var s = boxes.length;
   	var i;
   	for(i=0;i<s;i++) {
     	  var x = boxes[i].x;
          var y = boxes[i].y;
          var label = boxes[i].label;
          var params = boxes[i].params;
          var ret_type = boxes[i].ret_type;
          var loc = get_box_location(canvas,x,y,label,params,ret_type);
          if (popup_x>loc[0] && popup_x<loc[0]+loc[2] && popup_y>loc[1] && popup_y<loc[1]+loc[3]) {
	     codegen_num = 0;
	     codegen_map = { };
	     var str = codegen(boxes[i].id,values);
	     //console.log(str);
	   }
	   }
	   return;
     }

     // inside of popup
     // TODO add functionality based on clicknum
   }
   }

   var s = boxes.length;
   var i;
   for(i=0;i<s;i++) {
     var x = boxes[i].x;
     var y = boxes[i].y;
     var label = boxes[i].label;
     var params = boxes[i].params;
     var ret_type = boxes[i].ret_type;
     var loc = get_box_location(canvas,x,y,label,params,ret_type);
     if (pos.x>loc[0] && pos.x<loc[0]+loc[2] && pos.y>loc[1] && pos.y<loc[1]+loc[3]) {
       grabbed_object = boxes[i];
       grab_dx = pos.x-loc[0];
       grab_dy = pos.y-loc[1];
       }
     //console.log(x);
     //console.log(y);
   }
     e.preventDefault();
   }
   //console.log(pos);
}
function mouseUp(e)
{
   var canvas = document.getElementById('myCanvas');
   var pos = getMousePos(canvas,e);
//   console.log(pos);
   grabbed_object=null;
}
function popup_menu(popups, popuptexts, x,y)
{
  //console.log("Push");
  popups.push([x,y]);
  popuptexts.push("Properties");
  popuptexts.push("CodeGen");
  popuptexts.push("Settings");
  popuptexts.push("Display");

}
var popup_width = -1;
var popup_height = -1;
var popup_x = -1;
var popup_y = -1;
function draw_popup(canvas, x,y)
{
	    var ctx = canvas.getContext("2d");

	    var mg = 5;
	    var ht = 16+2;
	    var wd = 100;
	    var menu_items = ["Properties", "CodeGen", "Settings", "Display"];
	    var num = menu_items.length;
	    var hg = (mg+ht+mg)*num;
	    ctx.fillStyle = "#000000";
	    ctx.fillRect(x,y,wd,hg);
	    ctx.fillStyle = "darkslategray";
	    ctx.fillRect(x+2,y+2,wd-4,hg-4);

	    popup_x = x;
	    popup_y = y;
	    popup_width = wd;
	    popup_height = hg;
}
function draw_popup_highlight(canvas,x,y,mouse_pos)
{
   var ctx = canvas.getContext("2d");
   var mouse_x = mouse_pos[0];
   var mouse_y = mouse_pos[1];
   var wd = 100;
   var mg = 5;
   var ht = 16+2;
   var hg = mg+ht+mg;
   
   var num2 = Math.floor(mouse_y/(mg+ht+mg));
   var yy = num2*(mg+ht+mg);

   ctx.fillStyle = "#000000";
   ctx.fillRect(x,y+yy,wd,hg);
  // ctx.fillStyle = "dimgray";
  // ctx.fillRect(x+2,y+yy+2,wd-4,hg-4);


}
function draw_popup_texts(canvas,x,y,menu_items)
{
	    var ctx = canvas.getContext("2d");

	    var mg = 5;
	    var ht = 16+2;
	    var wd = 100;
	    var num = menu_items.length;
	    var hg = (mg+ht+mg)*num;
	    ctx.font = "16px Verdana";
	    var i;
	    for(i=0;i<num;i++) {
	      var menu_item = menu_items[i];
	       ctx.fillStyle = "lightgray";
	       ctx.fillText(menu_item, x + mg, y+ ht+ i*(mg+ht+mg));
	    }

}
function click_popup(canvas,x,y)
{
	    var mg = 5;
	    var ht = 16+2;

	    var num2 = Math.floor(y/(mg+ht+mg));
	    return num2;
	    
}
var tempnum = 0;
var selected_item = null;
function selectBox(e, tempboxes)
{
   var node = e.target.innerHTML;

   if (selected_item!=null) {
     selected_item.classList.remove('focus');
     selected_item.classList.add('label');
   }
   e.target.classList.add('focus');
   e.target.classList.remove('label');
   selected_item = e.target;

   //console.log(node);
   var obj = g_objs[node];
   //console.log(obj);

   var name = obj[0];
   var ret_type = obj[1];
   var apiname = obj[2];
   var funcname = obj[3];
   var count = obj[4];
   var i;
   var param_arr = [];
   var value_arr = [];
   var type_arr = [];
   for(i=0;i<count;i++) {
   	var p_name = obj[5+i*3+0];
	var p_type = obj[5+i*3+1];
	var p_def = obj[5+i*3+2];
	if ((p_type.length<=4 && p_type[0]>='A' && p_type[0]<'Z')||is_array(p_type))
        	param_arr.push(p_type);
	value_arr.push(p_def);
	type_arr.push(p_type);
   }
   tempboxes.value = { id:"temp" + tempnum.toString(), x:-1000, y:-1000, label:name, params:param_arr, ret_value:ret_type };
   created_object = tempboxes;
   created_value = { value : { id:"temp" + tempnum.toString(), params:value_arr, types:type_arr, func:funcname, api:apiname, ret:ret_type } };
   
   tempnum=tempnum+1;

   //console.log(created_value.value);

}

var codegen_num = 0;
var codegen_map = { };

function codegen(id, values)
{
  //console.log(id);
  var s = values.length;
  var i;
  for(i=0;i<s;i++) {
     var val = values[i];
     if (val.id == id) {
       break;
     }
  }
  var value = values[i];

  var params = value.params;
  var types = value.types;
  var i;
  var is = params.length;
  var res = "";
  var pp = [];
  var pu = [];
  for(i=0;i<is;i++) {
     var param = params[i];
     var type = types[i];
     //console.log("Type:" + type);
     if ((type.length<=4 && type[0]>='A' && type[0]<='Z')||is_array(type)) {
     	var ys = values.length;
	var y;
	var val=null;
	for(y=0;y<ys;y++) {
	  val = values[y];
	  //console.log("Compare:" + val.id + "=" + param);
	  if (striparray(val.id)==striparray(param)) break;
	}
	//console.log(val);
	var ret2 = val.ret;
	//console.log(ret2);
     	if (is_array(type)&&!is_array(ret2)) {
	   var arr = value_to_array(param);
	   var s = arr.length;
	   var j;
	   var pp2 = [];
	   for(j=0;j<s;j++) {
	      var pk = arr[j];
	      var str = codegen(pk,values);
	      res = res.concat(str);
	      pp2.push(codegen_map[pk]);
	      }
	      pp.push(pp2);
	      pu.push("A");
	} else {
          var str = codegen(striparray(param),values);
	  res = res.concat(str);
	  pp.push(codegen_map[striparray(param)]);
	  pu.push("V");
	}
     } else { pp.push(param); pu.push("V"); }
  }
    
  var api = value.api;
  var func = value.func;
  var ret = value.ret;
  
  var line = ret + " I" + codegen_num.toString() + "=ev." + api + "." + func + "(";
  var ss = pp.length;
  for(i=0;i<ss;i++) {
    if (pu[i]=="A") {
      line+="std::vector<";
      line+=striparray(types[i]);
      line+=">{";
      var ij;
      var s = pp[i].length;
      for(ij=0;ij<s;ij++) { line+=pp[i][ij]; if (ij!=s-1) line+=","; }
      line+="}";
    } else {
      line+=pp[i];
    }
      if (i!=ss-1) line+=",";
  }
  line+=");@"
  res+=line;
  codegen_map[id] = "I" + codegen_num.toString();
  codegen_num++;
  return res;
}
function close_display()
{
      var disp = document.getElementById("display");
      disp.style.display = "none";
}
function close_props(e,properties,values)
{
   var s = properties.length;
   var i;
   for(i=0;i<s;i++) {
      var id = properties[i].id;
      var num = properties[i].num;
      var value = properties[i].value;
      var ss = values.length;
      var j;
      for(j=0;j<ss;j++) {
         var val = values[j];
	 var iid = val.id;
	 if (iid==id) {
	    val.params[num] = value;
	 }
      }
   }
   while(properties.length>0) properties.pop();
   var el = document.getElementById('propertyclose');
   el.style.display = "none";
   var el = document.getElementById('table');
   el.style.display = "none";
}
function close_question(e, questionaire)
{
  gameapi_homepageurl = questionaire[0].value;
  g_project_name = questionaire[1].value;

  while(questionaire.length>0) questionaire.pop();
  var el = document.getElementById("questionaireclose");
  el.style.display = "none";
  var el = document.getElementById("qtable");
  el.style.display = "none";
}
window.onresize = resize_event;

function resize_size()
{
  var wd = window.innerWidth;
  var hd = window.innerHeight;
  if (g_ui_mode==1) {
     return [wd,hd];
  }
  var tg_wd = 800.0;
  var tg_hd = 600.0;
  var src_wd = wd-250.0-140.0;
  var src_hd = hd-50.0-130.0;
  var wd_factor = src_wd/830.0;
  var hd_factor = src_hd/650.0;
  var factor;
  if (wd_factor<hd_factor) factor = wd_factor;
  else factor = hd_factor;

  var sx = 830*factor;
  var sy = 650*factor;

  var ssx = 800*factor;
  var ssy = 600*factor;
  if (sx<100) sx=100;
  if (sy<100) sy=100;
  if (ssx<90) ssx=90;
  if (ssy<90) ssy=90;

  return [ssx,ssy];  
}

function resize_event(event) {
  var wd = window.innerWidth;
  var hd = window.innerHeight;
  var el = document.getElementById('sidecolumn');
  var el2 = document.getElementById('canvascolumn');
  var cv = document.getElementById('divcanvas');
  el.style.width = "150px";
  el2.style.width = (wd-150-2).toString() + "px";
  cv.style.width = (wd-150-2).toString() + "px";
  cv.style.height = (hd-2).toString()+"px";

  var elem = document.getElementById("display");


  //console.log("elem:" + elem);
  if (typeof(elem)!='undefined' && elem!=null) { } else { return; }
  if (g_ui_mode==1) {
     var closebutton = document.getElementById("close");
     //console.log("closebutton:" + closebutton);
     if (closebutton==null) return;
     closebutton.style.display = "none";
     closebutton.style.left = (250+sx-58).toString() + "px";
     closebutton.style.top = (50+sy+32).toString() + "px";
     var sx = wd;
     var sy = hd;

     elem.style.top = "0px";
     elem.style.left = "0px";
     elem.style.width = (sx).toString() + "px";
     elem.style.height = (sy).toString() + "px";
     var iframe = document.getElementById("ems");
     //  console.log("iframe:" + iframe);
     if (!iframe) return;
     iframe.style.width = (sx).toString() + "px";
     iframe.style.height = (sy).toString() + "px";
     var win = window.frames["ems"].contentWindow;
     //console.log("win:" + win);
     var cnv = win.document.getElementById("canvas");
     //  console.log("cnv:" + cnv);
     if (!cnv) return;
     cnv.style.width = (sx).toString() + "px";
     cnv.style.height = (sy).toString() + "px";

       if (g_emscripten_running && Module) {
	   try {
Module.ccall('set_resize_event', null, ['number', 'number'], [sx,sy], {async:true});
	   } catch(e) {
	     console.log(e);
	   }
	   }


    return;
  }

  var tg_wd = 800.0;
  var tg_hd = 600.0;
  var src_wd = wd-250.0-140.0;
  var src_hd = hd-50.0-130.0;
  var wd_factor = src_wd/830.0;
  var hd_factor = src_hd/650.0;
  var factor;
  if (wd_factor<hd_factor) factor = wd_factor;
  else factor = hd_factor;

  var sx = 830*factor;
  var sy = 650*factor;

  var ssx = 800*factor;
  var ssy = 600*factor;

  if (sx<100) sx=100;
  if (sy<100) sy=100;

  if (ssx<90) ssx=90;
  if (ssy<90) ssy=90;

  var closebutton = document.getElementById("close");
     //console.log("closebutton:" + closebutton);
  if (closebutton==null) return;
  closebutton.style.display = "block";
  closebutton.style.left = (250+sx-58).toString() + "px";
  closebutton.style.top = (50+sy+32).toString() + "px";

     elem.style.top = "50px";
     elem.style.left = "250px";
  elem.style.width = (sx+30).toString() + "px";
  elem.style.height = (sy+30).toString() + "px";
  var iframe = document.getElementById("ems");
       //console.log("iframe:" + iframe);
  if (!iframe) return;
  iframe.style.width = (sx+30).toString() + "px";
  iframe.style.height = (sy+30).toString() + "px";
  var win = window.frames["ems"].contentWindow;
  var cnv = win.document.getElementById("canvas");
       //console.log("cnv:" + cnv);
     if (!cnv) return;
  cnv.style.width = (ssx).toString() + "px";
  cnv.style.height = (ssy).toString() + "px";

       if (g_emscripten_running && Module) {
	   try {
Module.ccall('set_resize_event', null, ['number', 'number'], [ssx,ssy], {async:true});
	   } catch(e) {
	     console.log(e);
	   }
	   }

  //var cnv = document.getElementByTagName("canvas0");
//  console.log(cnv);
//console.log(win);
//console.log(wd);
}
var bd = document.getElementById("body");
bd.onload = function() { resize_event(null); }

var db;

function open_db(cont)
{
  if (!('indexedDB' in window)) {
    console.log('This browser doesn\'t support IndexedDB');
    return null;
  }
  var idb = window.indexedDB || window.webkitIndexedDB ||window.mozIndexedDB || window.msIndexedDB;
  const dbPromise = idb.open('gameapi', 23, function(upgradeDb) {
     console.log('checking if new indexdb entries needed');
	});
   console.log(dbPromise);
   dbPromise.onupgradeneeded = function() {
     var db = dbPromise.result;
       if (!db.objectStoreNames.contains('builderFiles')) {
        db.createObjectStore('builderFiles', {keyPath: 'name'});
	}
   };
   dbPromise.onsuccess = function(event) {
                     //console.log(event);
   		     var db = dbPromise.result;
		     cont(db);
		     }
   dbPromise.onerror = function(event) { console.log("ERROR"); }
}

function add_builder_file_to_db(db, itemname, file)
{
      var tx = db.transaction('builderFiles', 'readwrite');
      var store = tx.objectStore('builderFiles');
      var item = {
        name: itemname,
	value: file
	};
      store.add(item);
      return tx.complete;
}
// usage: var v = get_builder_file_from_db(promise,name);
//        v.then(function(val) { console.log(val); }
function get_builder_file_from_db(db, name)
{
    var tx = db.transaction('builderFiles', 'readonly');
    var store = tx.objectStore('builderFiles');
    var res = store.get(name);
    //console.log(res);
    return res;
}
function update_builder_file_to_db(db, itemname, itemfile)
{
      var tx = db.transaction('builderFiles', 'readwrite');
      var store = tx.objectStore('builderFiles');
      var item = {
        name: itemname,
	value: itemfile
	};
	var res = store.put(item);
	return res;
}
// to use this:
// var v = get_all_names_from_db(promise);
// v.then(function(items) { console.log(items); });
function get_all_names_from_db(db)
{
    var tx = db.transaction('builderFiles', 'readonly');
    var store = tx.objectStore('builderFiles');
    return store.getAll();
}

function store_builder_data(name, vm)
{
  var file = { boxes: vm.boxes, links: vm.links, values: vm.values };
  var dbPromise = open_db(function(db) {
    var v = get_all_names_from_db(db);
    v.onsuccess = function(e) {
        var items = e.target.result;
	//console.log(items);
        var is = items.length;
        var i;
        for(i=0;i<is;i++) {
          var item = items[i];
	  var itemname = item.name;
	  if (name==itemname) {
	     console.log("updating database item " + name + ".");
	     var vv = update_builder_file_to_db(db, name, file);
	     vv.onsuccess = function(e) {
	       console.log("update success!");
	       return;
	     }
	     return;
	  }
	  }
	       // here we didn't find the right item, so create it.
     	  console.log("database item " + name + " not found, so creating it.");
          add_builder_file_to_db(db, name, file);	 
     }
  });
}
function retrieve_builder_data(name, vm)
{
  var dbPromise = open_db(function(db) {

   var v = get_builder_file_from_db(db, name);
   v.onsuccess = function(e) {
      var res = e.target.result;
      if (res==null) { console.log("NULL"); return; }
      vm.boxes = res.value.boxes;
      vm.links = res.value.links;
      vm.values = res.value.values;

      // update tempnum
      var s = vm.boxes.length;
      var i;
      for(i=0;i<s;i++) {
         var id = vm.boxes[i].id;
	 var str = id.substr(4);
	 var n = Number(str);
	 if (tempnum<=n) tempnum=n+1;
      }
      console.log("Builder data retrieved!");
   }
   //console.log(v);
 });
}



</script>

</body>
</html>