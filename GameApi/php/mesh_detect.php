<?php

// usage: mesh_detect.php?next=foo.php&id=ksdnhsk
require_once("user.php");

$next = $_GET["next"];
$id = $_GET["id"];
$noheaders = $_GET["noheaders"];
$seamless = $_GET["seamless"];
$transparent = $_GET["transparent"];
$width = $_GET["width"];
$height = $_GET["height"];
$meshdata = $_GET["meshdata"];
$homepage = $_GET["homepage"];
if ($next=="") { $next = $_POST["next"]; }
if ($id=="") { $id = $_POST["id"]; }
if ($noheaders=="") { $noheaders = $_POST["noheaders"]; }
if ($seamless=="") { $seamless=$_POST["seamless"]; }
if ($transparent=="") { $transparent=$_POST["transparent"]; }
if ($width=="") { $width = $_POST["width"]; }
if ($height=="") { $height = $_POST["height"]; }
if ($meshdata=="") { $meshdata = $_POST["meshdata"]; }
if ($homepage=="") { $homepage = $_POST["homepage"]; }
$meshdata=json_encode($meshdata);
$arr = decode_id($id);
$user = $arr["username"];
$index = $arr["index"];
if ($width==NULL) {
  $width=800;
}
if ($height==NULL) {
  $height=600;
}
$arr2 = array("username" => $user,
              "index" => intval($arr["index"]),
  	      "num" => intval($arr["index"])+1,
	      "width" => $width,
	      "height" => $height);
echo "<html>";
echo "<head>";
viewport_meta();
twitter_meta( $arr2, 0);
google_meta( $arr2, 0);
echo "</head>";
echo "<body>";
?>
<div id="content2"></div>
<div id="content" name="content">We detected your browser settings:</div>
<script>
function detect_webassembly() {
  var filename="<?php echo $next ?>?id=<?php echo $id ?>";
  var postparam = "";
  var postparam_pthread = "ok";
  var postparam_noh = "<?php echo $noheaders ?>";
  var postparam_seam = "<?php echo $seamless ?>";
  var postparam_trans = "<?php echo $transparent ?>";
  var postparam_wd = "<?php echo $width ?>";
  var postparam_hg = "<?php echo $height ?>";
  var postparam_meshdata = <?php echo $meshdata ?>;
  var postparam_homepage = "<?php echo $homepage ?>";
  
  try {
     const buffer = new SharedArrayBuffer(8);
  } catch(err) {
     postparam_pthread = "fail";
  }

  if ('WebAssembly' in window) {
     postparam+="ok";
  } else {
     postparam+="fail";
  }
   var form = document.createElement('form');
  form.method = 'POST';
  form.action =  filename;
  //form.target='_blank';
  var input = document.createElement('input');
  input.type = 'hidden';
  input.name = 'webasm';
  input.value = postparam;
  form.appendChild(input);

  var input6g = document.createElement('input');
  input6g.type = 'hidden';
  input6g.name = 'pthread';
  input6g.value = postparam_pthread;
  form.appendChild(input6g);


  var input2 = document.createElement('input');
  input2.type = 'hidden';
  input2.name = 'noheaders';
  input2.value = postparam_noh;
  form.appendChild(input2);

  var input22 = document.createElement('input');
  input22.type = 'hidden';
  input22.name = 'seamless';
  input22.value = postparam_seam;
  form.appendChild(input22);

  var input23 = document.createElement('input');
  input23.type = 'hidden';
  input23.name = 'transparent';
  input23.value = postparam_trans;
  form.appendChild(input23);


  var input3 = document.createElement('input');
  input3.type = 'hidden';
  input3.name = 'width';
  input3.value = postparam_wd;
  form.appendChild(input3);

  var input4 = document.createElement('input');
  input4.type = 'hidden';
  input4.name = 'height';
  input4.value = postparam_hg;
  form.appendChild(input4);

  var input5 = document.createElement('input');
  input5.type = 'hidden';
  input5.name = 'meshdata';
  input5.value = postparam_meshdata;
  form.appendChild(input5);

  var input6 = document.createElement('input');
  input6.type = 'hidden';
  input6.name = 'homepage';
  input6.value = postparam_homepage;
  form.appendChild(input6);

  var inputjj = document.createElement('input');
  inputjj.type = 'submit';
  inputjj.name = 'submitbutton';
  inputjj.value = 'CLICK HERE TO CONTINUE';
  form.appendChild(inputjj);

  var content = document.getElementById("content");

  content.appendChild(form);
  form.submit();
}
detect_webassembly();
</script>
</body>
</html>