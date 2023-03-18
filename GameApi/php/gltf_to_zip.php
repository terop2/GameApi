<?php

?>

<html>
<h1>GLTF to HostingSpace ZIP Converter</h1>

<form method="POST" action="/gltf_to_zip_result.php">
<label class="url" for="gltffile">GLTF2.0 File Url:</label>
<input class="gltf" type="url" id="gltffile" name="gltffile" value=""><br>

(you just need .gltf file url, it'll find all dependencies starting from that file)
<br><br>

<input class="cb" type="checkbox" name="transparent" value="trans" checked><div class="label">Transparent</div><br>
<input class="cb" type="checkbox" name="zoom" value="zoom" checked><div class="label">Zoom</div><br>
<input class="cb" type="checkbox" name="rotate" value="rotate" checked><div class="label">Rotate</div><br>
<input class="cb" type="checkbox" name="pan" value="pan" checked><div class="label">Pan</div><br>
<p>
<input class="submit" type="submit" value="Submit">
</form>
<style>
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