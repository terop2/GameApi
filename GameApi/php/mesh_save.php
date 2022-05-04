<?php

require_once("user.php");

$data = $_GET["id"];
$id = $data;
$arr = decode_id( $data );

$user = $arr["username"];
$num = read_num( $user );
$meshdata = $_POST["meshdata"];
$homepage = $_POST["homepage"];
$label = $_POST["label"];
if ($meshdata == "")
{
  load_data( $arr );
}
$arr2 = array("username" => $user,
              "index" => intval($arr["index"]),
              "data" => $meshdata,
              "num" => intval($arr["index"])+1,
	       "width" => 800,
	       "height" => 600,
	       "homepage" => $homepage);
save_data($arr2);
save_homepage($arr2);
write_num( $arr2["username"], $arr2["index"] );

$pngfilename = "user_data/user_" . $user . "/screenshot" . $arr["index"] . ".webp";
create_image( $_POST["pngfilename"], 280, 210, $pngfilename );
unlink($_POST["pngfilename"]);
set_label($arr2, $label);
header("Location: mesh.php?id=$id");
