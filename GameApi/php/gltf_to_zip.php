<?php

  $logstr = $_SERVER["HTTP_REFERER"];
if ($logstr != "https://meshpage.org/process.php" && $logstr != "")
{
$fp = fopen("./pp2/referer.log","a+");
fwrite($fp, "CONVERTER " . $logstr);
fclose($fp);
}

?>

<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, shrink-to-fit=no"/>
      <title>GLTF to HTML5 ZIP Converter</title>
</head>
<h1>GLTF to HTML5 ZIP Converter</h1>
<div class="moveright">+3d engine</div>
<div class="moveup">
(gltf file get prepared for hosting/displaying/rendering on the web)
</div>

<form method="POST" action="/gltf_to_zip_result.php">
<label class="url" for="gltffile">GLTF2.0 File Url:</label>
<input class="gltf" type="url" id="gltffile" name="gltffile" value="" onchange="file_change()"><br>

(you just need .gltf file url, it'll find all dependencies starting from that file)<br>
(or if you got the content from sketchfab, their gltf zip files are acceptable too)
<br><br>

<input id="transparent" class="cb" type="checkbox" name="transparent" value="trans" onchange="trans_change()"><div class="label">Transparent Canvas</div><br>
<input id="bigscreen" class="cb" type="checkbox" name="bigscreen" value="bigscreen"><div class="label">Bigscreen Support</div><br>
<input id="shadow" class="cb" type="checkbox" name="shadow" value="shadow" onchange="shadow_change()"><div class="label">Shadow</div><br>
<input id="anim" class="cb" type="checkbox" name="anim" value="anim" onchange="anim_change()"><div class="label">Gltf Animation</div><br>
<input id="large" class="cb" type="checkbox" name="large" value="large" onchange="large_change()"><div class="label">Large Model</div><br>
<input id="sketchfab" class="cb" type="checkbox" name="sketchfab" value="sketchfab" onchange="sketchfab_change()"><div class="label checked">Sketchfab zip</div><br>
<input id="zoom" class="cb" type="checkbox" name="zoom" value="zoom" checked><div class="label">Zoom</div><br>
<input id="rotate" class="cb" type="checkbox" name="rotate" value="rotate" checked><div class="label">Rotate</div><br>
<!--input class="cb" type="checkbox" name="pan" value="pan" checked><div class="label">Pan</div><br-->
<p>
<input class="submit" type="submit" value="Convert to HTML5">
<?php echo file_get_contents("./pp2/tmp.count") ?>
</form>

<img id="trans" style="position:absolute; top: 200px; left:300px;" width="320px" height="200px" src="https://meshpage.org/ab_black.png"/>

<script>
function trans_change()
{
  var el = document.getElementById("transparent");
  if (el.checked==true)
  {
     var el2 = document.getElementById("trans");
     el2.src="https://meshpage.org/ab_transparent.png";
  } else {
     var el2 = document.getElementById("trans");
     el2.src="https://meshpage.org/ab_black.png";
  }
}
function file_change()
{
  //var el3 = document.getElementById("gltffile");
  //var name = el3.value;
  //var ext = name.substring(name.length-4);
  //if (ext==".zip") {
  //  var el3 = document.getElementById("sketchfab");
  //  el3.disabled=false;
  //} else {
  //  var el3 = document.getElementById("sketchfab");
  //  el3.disabled=true;
  //  el3.checked=true;
  //  sketchfab_change();
  //}
}
function sketchfab_change()
{
  var el3 = document.getElementById("sketchfab");
  if (el3.checked==true) {
  var el2aaa = document.getElementById("transparent");
  el2aaa.disabled=false;
  var el2aa = document.getElementById("bigscreen");
  el2aa.disabled=false;
  var el2a = document.getElementById("shadow");
  el2a.disabled=false;
  var el2b = document.getElementById("anim");
  el2b.disabled=false;
  var el2c = document.getElementById("large");
  el2c.disabled=false;
  var el2d = document.getElementById("zoom");
  el2d.disabled=false;
  var el2e = document.getElementById("rotate");
  el2e.disabled=false;
  } else {
  var el2aaa = document.getElementById("transparent");
  el2aaa.disabled=true;
  var el2aa = document.getElementById("bigscreen");
  el2aa.disabled=true;
  var el2a = document.getElementById("shadow");
  el2a.disabled=true;
  var el2b = document.getElementById("anim");
  el2b.disabled=true;
  var el2c = document.getElementById("large");
  el2c.disabled=true;
  var el2d = document.getElementById("zoom");
  el2d.disabled=true;
  var el2e = document.getElementById("rotate");
  el2e.disabled=true;
  }
}
function anim_change()
{
  var el3 = document.getElementById("shadow");
  if (el3.checked==true) {

  var el = document.getElementById("anim");
  if (el.checked==true)
  {
  var el2 = document.getElementById("large");
  el2.checked=false;
  }
  }
}
function large_change()
{
  var el3 = document.getElementById("shadow");
  if (el3.checked==true) {
  var el = document.getElementById("large");
  if (el.checked==true)
  {
  var el2 = document.getElementById("anim");
  el2.checked=false;
  }
}
}
function shadow_change()
{
  var el3 = document.getElementById("shadow");
  if (el3.checked==true) {
  var el = document.getElementById("large");
  var el2 = document.getElementById("anim");
  if (el.checked && el2.checked)
  {
  el2.checked=false;
  }
}
}
</script>

HINT#1: Firsttime users should check the server config from our faq: <a href="https://meshpage.org/meshpage_5">faq</a>.<br>
HINT#2: For advanced users, there's a builder tool available at: <a href="https://meshpage.org/meshpage_4">builder tool</a>.<br>
HINT#3: &lt;iframe scrolling="no" src="index.html" width="830" height="630"/&gt; to embed it to your main article.<br>
HINT#4: Changing the canvas dimensions is possible if you modify both the embed and index.html<br>
HINT#5: You can get the source code (needed for LGPL/GPL) from <a href="https://github.com/terop2/GameApi">https://github.com/terop2/GameApi</a>.<br>
HINT#6: Urls from google drive doesn't seem to be working, its missing .zip or .glb or .gltf extension in the url and is usually password protected.<br>
<style>
.moveright {
   position: relative;
   top: -42px;
   left: 470px;
   display: inline-block;
}
.moveup {
   position: relative;
   top: -20px;
   left: -50px;
   display: inline-block;
}
.gltf {
   width: 50%;
   display: inline-block;
  font-size: 150%;
}
.cb {
  font-size: 150%;
/* Double-sized Checkboxes */
  -ms-transform: scale(2); /* IE */
  -moz-transform: scale(2); /* FF */
  -webkit-transform: scale(2); /* Safari and Chrome */
  -o-transform: scale(2); /* Opera */
  transform: scale(2);
  padding: 10px;
}
.label {
  font-size: 150%;
  display: inline;
  margin-left: 8px;
}
.url {
  font-size: 150%;
  display: inline;
}
.submit {
  font-size: 150%;
}
</style>
</html>