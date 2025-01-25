<?php

require_once("user.php");

$data = $_GET["id"];
$full = $_GET["full"];
$arr2 = array("username" => "terop",
      	      "index" => intval($data),
	      "num" => intval($data)+1,
	      "width" => 800,
	      "height" => 500);
if ($full=="true") {
$apk = get_apk( $arr2 );
} else {
$apk = get_apk_status( $arr2 );
}
if ($full=="true") {
    header("Content-Type: vnd.android.package-archive");
    header("Content-Disposition: attachment; filename=\"android$data.apk\"");
    }
echo "$apk";
