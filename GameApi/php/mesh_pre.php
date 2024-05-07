<?php

require_once("user.php");

$data = $_GET["id"];
$arr = decode_id( $data );
$user = $arr["username"];
$arr2 = array("username" => $user,
              "index" => intval($arr["index"]),
	      "num" => intval($arr["index"])+1,
	      "width" => 800,
	      "height" => 500);
$code = user_code( $arr2, 0 );
header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
header("Cache-Control: post-check=0, pre-check=0", false);
echo "$code";