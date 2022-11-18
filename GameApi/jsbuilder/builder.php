<?php

?>
<html>
<head>
</head>
<body>
<div id="menu"></div>
</body>
</html>
<style>
.titlebutton {
  width: 200px;
}
.elembutton {
  width:200px;
  background-color: #888;
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

function menu_widgets(titles, arr)
{
   var menu = document.getElementById("menu");
   var s = arr.length;
   var res="";
   for(var i=0;i<s;i++)
   {
	res+="<button class='titlebutton' style='width:200px;' type='button' onclick='toggle(" + i.toString() + ")'>";
	res+=titles[i];
	res+="</button>";
	res+="<div id='sec" + i.toString() +"' style='display:none'>";

	var obj = arr[i];
	for(var key in obj) {
	   var value = obj[key];
	   res+="<button class='elembutton' style='width:200px;' type='button'>";
	   res+=key;
	   res+="</button>";
	}
	res+="</div>";
	console.log(obj);

   }
   menu.innerHTML = res;
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


</script>