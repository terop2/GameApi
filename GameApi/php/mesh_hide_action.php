<?php

require_once("user.php");

page_header(6);
page_title("Vault Result", "");
$user = "terop";
$num = read_num( $user );
var_dump("SETHIDDEN/LOWMEM");
for($i = 0;$i<$num+1;$i++)
{
$arr = array("username" => $user,
             "index" => $i);

}
//var_dump($_FILES);
var_dump("CHECK FILES...");
foreach( $_FILES as $key => $val) {
//var_dump($val);
$key = substr($key, 3);
$pngf = "png" . $key;
//var_dump($pngf);

$pngfilename = $_FILES[$pngf]["tmp_name"];
//var_dump($pngfilename);
if ($pngfilename != "")
{
  $tmpfile = "user_data/user_" . $user . "/screenshot_tmp" . $key . ".png";
  $pngfile = "user_data/user_" . $user . "/screenshot" . $key . ".webp";
  echo "Creating $key as ($pngfilename,$tmpfile,$pngfile)";
  move_uploaded_file($pngfilename, $tmpfile);
  unlink($pngfile);
  create_image( $tmpfile, 280,210, $pngfile);
  unlink($tmpfile);
  echo "<img width=\"200\" height=\"150\" src=\"" . $pngfile . "\"/>";

  //var_dump("create image" . $pngfile);
}

}
//var_dump($_POST);

//var_dump("CHECK POST...");

foreach( $_POST as $key => $value )
{
echo "KEY:" . $key . " Value:" . $value . "\n";
$box4 = substr($key,-5);
if ($box4=="_hide") {
  $keystr = substr($key, 0, -5);
  $keynum = intval($keystr);
  $arr = array("username" => $user,
       	       "index" => $keynum);
  if ($keynum==0) continue;
  if ($value == "on") {
    echo "HIDE";
    set_hidden($arr,1);
    } else if ($value=="off") {
    echo "SHOW";
    set_hidden($arr,0);
    }
}
$box3 = substr($key,-8);
if ($box3=="_addtext") {
   $keystr = substr($key,0,-8);
   $keynum = intval($keystr);
   $arr = array("username" => $user,
   	  	"index" => $keynum);
   if ($keynum==0) continue;
   set_addtext($arr, $value);
} else {
$box2 = substr($key,-6);
if ($box2=="_label") {
   $keystr = substr($key,0,-6);
   $keynum = intval($keystr);
   $arr = array("username" => $user,
   	        "index" => $keynum);
  if ($keynum==0) continue;
  set_label($arr, $value);
} else {

$box = substr($key, -7);
if ($box=="_lowmem") {
   $keystr = substr($key,0,-7);
   $keynum = intval($keystr);
   $arr = array("username" => $user,
                "index" => $keynum);
   if ($keynum==0) continue;
   if ($value == "on") {
      set_lowmem($arr,1);
   } else if ($value=="off") {
     set_lowmem($arr,0);
   }
} else {
//$keynum = intval($key);
//$arr = array("username" => $user,
//             "index" => $keynum);

//if ($keynum==0) continue;
//var_dump($value);
//if ($value=="on") {
//  echo "$i hidden.";
//  set_hidden($arr, 1);
//  $i = $arr["index"];
//  $filename = "user_data/user_" . $user . "/screenshot" . $i . ".webp";
//  echo "<img width=\"200\" height=\"150\" src=\"" . $filename . "\"/>";

//} else {
//set_hidden($arr,0);
//}
}
}
}
}
