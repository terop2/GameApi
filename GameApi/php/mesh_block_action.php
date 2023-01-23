<?php
require_once("user.php");
$arr = choose_user();
if ($arr["user"] != "terop_priv" && $arr["user"]!="henri_priv") { die(); }

$id = $_POST["idelem"];
$val = $_POST["block"];

$arr2 = decode_id( $id );

if ($val=="block") {
   write_url_block( $arr2, "block" );
   echo "BLOCKED!";
} else if ($val=="none") {
   write_url_block( $arr2, "none" );
   echo "RELEASED!";
} else {
  echo "FAILED!";
}

