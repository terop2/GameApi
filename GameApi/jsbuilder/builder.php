<?php

?>
<html>
<head>
</head>
<body class="page">
<div id="hidden_widgets1" style="display:none"></div>
<div id="hidden_widgets2" style="display:none"></div>
<canvas id="floating" class="float"></canvas>
<div horizontal layout start-justified class="horiz">
   <div class="leftpane">
     <div id="menu"></div>
   </div>
   <div class="rightpane">
     <canvas id="maincanvas" class="canvas"></canvas>
   </div>
</div>
</body>
</html>
<style>
.float {
   position:absolute;
}
.page {
      background-color: #000;
}
.rightpane {
   width: 80%;
   float: left;
}
.canvas {
   border: 0px solid #fff; 
   width: 800px;
   height: 600px;
   image-rendering: -webkit-optimize-contrast;
   image-rendering: crisp-edges;
}
.horiz {
  
}
::-webkit-scrollbar {
   background: #000;
}
::-webkit-scrollbar-track {
   background: #000;
   border: 2px solid white;
}
::-webkit-scrollbar-thumb {
   background: #fff;
   border: 0px solid #fff;
}
::-webkit-scollbar-thumb:hover {
   background: #888;
}
.leftpane {
   float: left;
  width: 200px;
  height: 100%;
  overflow-y: scroll;
  overflow-x: hidden;

}
.titlebutton {
  width: 180px;
  text-align: left;
  font-size: 20px;
  //font-weight: bold;
  background-image: linear-gradient(#842,#210);
  border: 2px inset #421;
   color: #fff;
}
.titlebutton:hover {
   border: 2px solid white;		   
   color: #fff;
}
.elembutton {
  width:180px;
  background-color: #000;
  text-align: left;
  font-size: 15px;
  font-weight: bold;
  border: 1px solid black;
  color: #fff;
}
.elembutton:hover {
   background-color: #000;
   color: #fff;
   border: 1px solid white;
}
</style>
<script>
function menu_filenames() {
  return new Promise((accept,reject) => {
  var count_filename="menu_count.txt";
  fetch(count_filename).then(response => {
     response.body.getReader().read().then(value => {
     	//console.log(value);
	var s = value.value.length;
	var ss = "";
	for(var i=0;i<s;i++) {
        var s2 = String.fromCharCode(value.value[i]);
	ss+=s2;
	}
	var s3 = Number(ss);	

  	var filenames = [];
  	for(var i=0;i<s3;i++) {
	   filenames.push("menu"+i.toString()+".txt");
  	}
	accept(filenames);
	}
	); }); });
}

function menu_contents(filenames) {
   return new Promise((accept,reject) => {
   	  var s = filenames.length;
	  var arr=[];
	  for(var i=0;i<s;i++)
	  {
	     var one = new Promise((accept2,reject2) => {
	          var filename = filenames[i];
		  //console.log(filename);
		  fetch(filename).then(response => {
	          response.body.getReader().read().then( value => {
		      var json = value.value;
		      //console.log(json);
		      var s = value.value.length;
		      var ss = ""; 
		      for(var i=0;i<s;i++) {
        	      var s2 = String.fromCharCode(value.value[i]);
		      ss+=s2;
		      }

		      var obj = JSON.parse(ss);
		      accept2(obj);
		      });});});
	       arr.push(one);
	  }
	  Promise.all(arr).then((values) => accept(values));

   });
}

var g_builder_node_data=[];

function menu_widgets(titles, arr)
{
   g_builder_node_data = arr;
   var menu = document.getElementById("menu");
   var s = arr.length;
   var res="";
   for(var i=0;i<s;i++)
   {
	res+="<button class='titlebutton' type='button' onclick='toggle(" + i.toString() + ")'>";
	res+=titles[i];
	res+="</button>";
	res+="<div id='sec" + i.toString() +"' style='display:none'>";

	var obj = arr[i];
	var j = 0;
	for(var key in obj) {
	   var value = obj[key];
	   //console.log(value);
	   res+="<button class='elembutton' onclick='click_handler(" + i.toString() +",\"" + key.toString() + "\")' type='button'>";
	   res+=key;
	   res+="</button>";
	   j++;
	}
	res+="</div>";
	//console.log(obj);

   }
   menu.innerHTML = res;
}

function click_handler(i,j)
{
   var val = g_builder_node_data[i][j];
   //console.log(val);
   start_drag(val);
}

var state = {};

function toggle(i)
{
	var wid = document.getElementById("sec" + i.toString());
	if (state[i]!="0")
	   { wid.style=""; state[i]="0"; }
	   else { wid.style="display:none;"; state[i]="1"; }
}

var titles = ["BitmapApi","BoolBitmapApi", "FloatBitmapApi", "PolygonApi", "MaterialsApi", "ShaderApi", "LinesApi", "PointsApi", "MoveApi", "PointApi", "VectorApi", "VolumeApi", "FloatVolumeApi", "CovApi", "FontApi", "TextureApi", "BooleanOps", "PDApi", "WaveformApi", "MainLoopApi", "FrameBufferApi"];

var filenames = menu_filenames();
filenames.then((value)=> {
   //console.log(value);
   menu_contents(value).then((value) => {
   	menu_widgets(titles, value);
   });
});


var node = { label: "testi", ret_type : "BM", param_types : ["BM","BM","BM"], param_labels : ["a1","a2","a3"] };

var hidden1 = new Map();
var hidden2 = new Map();

function post_hidden(i)
{
   var elem = document.getElementById("hidden_widgets" + i.toString());
   var res="";
   //console.log(hidden);
   var hidden = hidden1;
   if (i==2) hidden=hidden2;
   for(const [key,value] of hidden.entries()) {
      res+=value;
   }
   elem.innerHTML = res;
}

const node_width=70;
const node_title_height=15;
const node_height=60;

var uid=0;
function canvas_gameapi_grads()
{
    var scale=6;
   uid++;

   hidden1.set("grad2" + uid.toString(), "<canvas id='grk" + uid.toString() + "' width='300px' height='300px' style='image-rendering: crisp-edges;  object-fit: contain;'></canvas>");
   hidden1.set("grad" + uid.toString(), "<canvas id='grd" + uid.toString() + "' width='300px' height='45px' style='image-rendering: crisp-edges; object-fit: contain;'></canvas>");
   post_hidden(1);
   var canvas2 = document.getElementById("grd" + uid.toString());
   canvas2.width = node_width*scale;
   canvas2.height = node_title_height*scale;
   var canvas3 = document.getElementById("grk" + uid.toString());
   canvas3.width = node_width*scale;
   canvas3.height = (node_height-node_title_height)*scale;
   var ctx2 = canvas2.getContext("2d");
   var ctx3 = canvas3.getContext("2d");
   var grd = ctx2.createLinearGradient(0,0,node_width*scale,node_title_height*scale);
   grd.addColorStop(0,"#632");
   grd.addColorStop(1,"#421");
   ctx2.fillStyle=grd;
   ctx2.fillRect(0,0,node_width*scale,node_title_height*scale);
   ctx2.fillStyle="#842";
   ctx2.fillRect(0,0,node_width*scale,1*scale);
   ctx2.fillStyle="#842";
   ctx2.fillRect(0,0,1*scale,node_title_height*scale);
   var grd2 = ctx3.createLinearGradient(0,0,node_width*scale,(node_height-node_title_height)*scale);
   grd2.addColorStop(0,"#842");
   grd2.addColorStop(1,"#421");
   ctx3.fillStyle=grd2;
   ctx3.fillRect(0,0,node_width*scale,(node_height-node_title_height)*scale);
   return { "canvas2" : canvas2, "canvas3" : canvas3 };
}
function canvas_gameapi_node(node)
{
    var scale=6;
   hidden2.set("img" + uid.toString(),"<canvas id='cnv" + uid.toString() + "' width='300px' height='45px' style='image-rendering: crisp-edges; object-fit: contain;'></canvas>");

   post_hidden(2);
   var canvas = document.getElementById("cnv" + uid.toString());
   canvas.width = node_width*scale;
   canvas.height = node_title_height*scale;

   var ctx = canvas.getContext("2d");
   ctx.fillStyle="#fff";
   ctx.font = (node_title_height*scale).toString() + "px bold";
   ctx.fillText(node.label, (node_title_height-3)*scale,(node_title_height-3)*scale);

   return { "canvas" : canvas };
}
function render_drag_node(node_data, pos)
{
    var target_scale=1;
    var orig_scale=6;
    var main = document.getElementById("floating");
    main.width=node_width*target_scale;
    main.height=node_height*target_scale;
    var ctx = main.getContext("2d");
    var scale2=6;
    //ctx.setTransform(1,0,0,1,0,0);
    var img1 = node_data.canvas2;
    var img2 = node_data.canvas;
    var img3 = node_data.canvas3;
    ctx.drawImage(img1,0,0,node_width*orig_scale,node_title_height*orig_scale,pos["x"],pos["y"],node_width*target_scale,node_title_height*target_scale);
    ctx.drawImage(img2,0,0,node_width*orig_scale,node_title_height*orig_scale,pos["x"],pos["y"],node_width*target_scale,node_title_height*target_scale);
    ctx.drawImage(img3,0,0,node_width*orig_scale,(node_height-node_title_height)*orig_scale,pos["x"],pos["y"]+node_title_height*target_scale,node_width*target_scale,(node_height-node_title_height)*target_scale);
}
function render_gameapi_node(node_data, pos)
{
    var target_scale=6;
    var orig_scale=6;
    var main = document.getElementById("maincanvas");
    var ctx = main.getContext("2d");
    var scale2=6;
    //ctx.setTransform(1,0,0,1,0,0);
    var img1 = node_data.canvas2;
    var img2 = node_data.canvas;
    var img3 = node_data.canvas3;
    ctx.drawImage(img1,0,0,node_width*orig_scale,node_title_height*orig_scale,pos["x"],pos["y"],node_width*target_scale,node_title_height*target_scale);
    ctx.drawImage(img2,0,0,node_width*orig_scale,node_title_height*orig_scale,pos["x"],pos["y"],node_width*target_scale,node_title_height*target_scale);
    ctx.drawImage(img3,0,0,node_width*orig_scale,(node_height-node_title_height)*orig_scale,pos["x"],pos["y"]+node_title_height*target_scale,node_width*target_scale,(node_height-node_title_height)*target_scale);
}


var drag_node = {};

function drag_move(event)
{
   var f = document.getElementById("floating");
   f.style.left = event.clientX.toString() + "px";
   f.style.top = event.clientY.toString() + "px";
   drag_node["x"] = event.clientX;
   drag_node["y"] = event.clientY;
   console.log(drag_node);
}

var click_flag=0;

function drag_click_handler(event)
{
      if (click_flag==0) { click_flag=1; } else {
      console.log("END DRAG");
      document.removeEventListener('mousemove', drag_move);
      document.removeEventListener('click', drag_click_handler);
      click_flag=0;
      }
}

function param_types(node)
{
   var s = node[4];
   var pos = 5;
   var res = [];
   
   for(var i=0;i<s;i++) {
      res.push(node[pos+1]);
      pos+=3;
   }
   return res;
}
function param_names(node)
{
   var s = node[4];
   var pos = 5;
   var res = [];
   
   for(var i=0;i<s;i++) {
      res.push(node[pos]);
      pos+=3;
   }
   return res;
}

function param_default_vals(node)
{
   var s = node[4];
   var pos = 5;
   var res = [];
   
   for(var i=0;i<s;i++) {
      res.push(node[pos+2]);
      pos+=3;
   }
   return res;
}


function start_drag(node)
{
   console.log("START_DRAG");
   console.log(node);
   drag_node["node"] = node;

   var nd = { label: node[0],
              ret_type: node[1],
	      param_types: param_types(node),
	      param_labels: param_names(node)
	      };

   var n0 = canvas_gameapi_grads();
   var n = canvas_gameapi_node(nd);
   var cc = Object.assign({},n0,n);
   render_drag_node(cc,{"x":0,"y":0});
   document.addEventListener('click', drag_click_handler);
   document.addEventListener('mousemove', drag_move);
}


var main = document.getElementById("maincanvas");
main.width*=6*2;
main.height*=6*2;
var n0 = canvas_gameapi_grads();
var n = canvas_gameapi_node(node);
render_gameapi_node(Object.assign({},n0,n), {"x":30*6,"y":30*6});
</script>