<?php

function decode( $str ) {
  $len = strlen( $str );
  $sbin = "";
  for( $i = 0; $i < $len; $i += 2 ) {
    $sbin .= pack( "H*", substr( $str, $i, 2) );
  }
  return $sbin;
}

require_once("user.php");

$user = $_POST["user"];
$key = $_POST["key"];
$data = $_POST["data"];

$var = read_num( $user );

$decoded_data = decode($data);

$arr = array("username" => $user,
             "screenshot_id" => $key,
	     "data" => $decoded_data,
	     "index" => $var);
save_screenshot( $arr );


var_dump($_POST);