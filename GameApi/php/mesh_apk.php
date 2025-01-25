<?php

require_once("user.php");

$data = $_GET["id"];
$full = $_GET["full"];
$arr2 = array("username" => "terop",
      	      "index" => intval($data),
	      "num" => intval($data)+1,
	      "width" => 800,
	      "height" => 500);
if ($full=="array") {
   $s = 650;
   $resarr = [];
   for($i=0;$i<$s;$i++)
   {
   $arr3= array("username" => "terop",
      	      "index" => intval($i),
	      "num" => intval($i)+1,
	      "width" => 800,
	      "height" => 500);
	$apk = get_apk_status( $arr3 );
	$resarr[$i]=$apk;
   }
   echo json_encode($resarr);
   exit;
} else {
if ($full=="true") {
$apk = get_apk( $arr2 );
} else {
$apk = get_apk_status( $arr2 );
}
}
if ($full=="true") {
    header("Content-Type: vnd.android.package-archive");
    header("Content-Disposition: attachment; filename=\"android$data.apk\"");
    }
echo "$apk";
