<?php

require_once("user.php");

$data = $_GET["id"];
$full = $_GET["full"];
$arr = decode_id( $data );
$user = $arr["username"];
$arr2 = array("username" => $user,
      	      "index" => intval($arr["index"]),
	      "num" => intval($arr["index"])+1,
	      "width" => 800,
	      "height" => 500);
if ($full=="array") {
   $s = 650;
   $resarr=[];
   for($i=0;$i<$s;$i++)
   {
   $arr3 =  array("username" => "terop",
      	      "index" => intval($i),
	      "num" => intval($i)+1,
	      "width" => 800,
	      "height" => 500);
   $val = get_addtext( $arr3 );
   $resarr[$i]=$val;	       
   }
   $addtext = json_encode($resarr);
} else {
  $addtext = get_addtext( $arr2 );
}
echo "$addtext";
