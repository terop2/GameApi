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
$bg = get_background( $arr2 );
echo "$bg";