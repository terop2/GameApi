<?php

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
<input class="gltf" type="url" id="gltffile" name="gltffile" value=""><br>

(you just need .gltf file url, it'll find all dependencies starting from that file)<br>
(or if you got the content from sketchfab, their gltf zip files are acceptable too)
<br><br>

<input class="cb" type="checkbox" name="transparent" value="trans"><div class="label">Transparent Canvas</div><br>
<input class="cb" type="checkbox" name="bigscreen" value="bigscreen"><div class="label">Bigscreen Support</div><br>
<input id="shadow" class="cb" type="checkbox" name="shadow" value="shadow" onchange="shadow_change()"><div class="label">Shadow</div><br>
<input id="anim" class="cb" type="checkbox" name="anim" value="anim" onchange="anim_change()"><div class="label">Gltf Animation</div><br>
<input id="large" class="cb" type="checkbox" name="large" value="large" onchange="large_change()"><div class="label">Large Model</div><br>
<input class="cb" type="checkbox" name="zoom" value="zoom" checked><div class="label">Zoom</div><br>
<input class="cb" type="checkbox" name="rotate" value="rotate" checked><div class="label">Rotate</div><br>
<!--input class="cb" type="checkbox" name="pan" value="pan" checked><div class="label">Pan</div><br-->
<p>
<input class="submit" type="submit" value="Convert to HTML5">
</form>

<script>
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

Firsttime users should check the server config from our faq: <a href="https://meshpage.org/meshpage_5">faq</a>.<br>
For advanced users, there's a builder tool available at: <a href="https://meshpage.org/meshpage_4">builder tool</a>.<br>
HINT#1: &lt;embed src="index.html" width="830" height="630"/&gt; to embed it to your main article.<br>
HINT#2: Changing the canvas dimensions is possible if you modify both the embed and index.html
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