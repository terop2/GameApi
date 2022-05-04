<?php

require_once("user.php");

$pngfilename = $_FILES["pngfilename"]["tmp_name"];
$id = $_GET["id"];
$noheaders = $_GET["noheaders"];
$width = $_GET["width"];
$height = $_GET["height"];
$label = $_POST["label"];

if ($noheaders=="") { $noheaders = $_POST["noheaders"]; }
if ($width=="") { $width = $_POST["width"]; }
if ($height=="") { $height = $_POST["height"]; }
//var_dump( $id );
$arr = decode_id( $id );

$user = $arr["username"];
$num = read_num( $user );
$meshdata = $_POST["meshdata"];
$homepage = $_POST["homepageurl"];



$pngfilename = "user_data/user_" . $user . "/screenshot_tmp" . $arr["index"] . ".png";
if ($_FILES["pngfilename"]["tmp_name"] != "")
{
move_uploaded_file($_FILES["pngfilename"]["tmp_name"], $pngfilename);
}
$width = 800;
$height = 600;

$arr2 = array("username" => $user,
              "index" => intval($arr["index"]),
  	      "num" => intval($arr["index"])+1,
	      "width" => $width,
	      "height" => $height);
//js_session_start();
//webassembly_detect( $arr2, $id, "mesh_preview.php", $noheaders, $width, $height, $meshdata, $homepage, $label );

if ($meshdata == "")
{
  header("Location: mesh.php?id=$id");
}
page_header(1);
//var_dump( $meshdata );

$arr = array("username" => $user,
               "index" => $arr["index"],
               "data" => $meshdata,
  	       "num" => $arr["index"]+1,
	       "width" => 800,
	       "height" => 600,
	       "homepage" => $homepage);
?>
<div style="height:80;">
<div style="font-size:50;">Previewing mesh</div>
</div>
<div style="width:300; float: left;">
<form style="overflow:hidden;" action="mesh_save.php?id=<?php echo $id ?>" method="post">
<textarea name="meshdata" rows="30" cols="80" style="display:none"><?php echo $meshdata ?></textarea>
<input type="submit" value="Save and Publish" style="width:300; height:50; font-size:30;"/>
<textarea name="pngfilename" style="display:none"><?php echo $pngfilename ?></textarea>
<textarea name="homepage" style="display:none"><?php echo $homepage ?></textarea>
<textarea name="label" rows="30" cols="80" style="display:none"><?php echo $label ?></textarea>
</form>
<form style="overflow:hidden;" action="create_mesh.php" method="post">
<textarea name="meshdata" rows="30" cols="80" style="display:none"><?php echo $meshdata ?></textarea>
<input type="submit" value="Cancel" style="width:300; height:50; font-size:30;"/>
</form>
</div>
<div style="left-margin: 330px;">
<img width="200" height="150" src="<?php echo $pngfilename ?>" />
</div>

<?php
//var_dump( $arr["data"] );
frame( $arr, $arr["width"], $arr["height"], $arr["data"], 0, $arr["homepage"], 0 );
page_footer();
?>