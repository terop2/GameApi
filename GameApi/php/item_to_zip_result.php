<?php
ini_set('memory_limit', '1024M');
require_once("user.php");


function set_status($val)
{
   file_put_contents("./pp2/tmp.zip.status", strval($val));
}

$itemnum = $_GET["itemnum"];
$itemid = $_GET["itemid"];

$arr = decode_id( $itemid );

$arr2 = array("username" => $arr["username"],
      	      "index" => intval($arr["index"]),
	      "num" => intval($arr["index"])+1
	      );

$code = user_code($arr2, 0);

file_put_contents("./pp2/tmp.txt", $code);
file_put_contents("./pp2/tmp.zip", "");

$homepage = load_homepage($arr2);
$label = get_label($arr2);
$label = str_replace(" ","_",$label);

$addtext = get_addtext($arr2);

$cmd = "(cd ./pp2;LD_LIBRARY_PATH=.. nohup ../deploytool --file ./tmp.txt -o ./tmp.zip --homepage " . $homepage . " > test.txt 2>&1)";

shell_exec($cmd);

set_status(8);

$file3 = file_get_contents("./pp2/test.txt");
$stat = stat("./pp2/test.txt");

$res = str_contains($file3, "ALL OK");

$sys = 0;
$attr = 0;

if ($res) {
  $sz = filesize("./pp2/tmp.zip");
  if ($sz < 950000000) {


  set_status(9);
  $za = new ZipArchive;
  $za->open("./pp2/tmp.zip",0);
  $idx = $za->locateName("license.html");
  $za->deleteIndex($idx);
  //$za->deleteName("license.html");
  $za->addFromString("license.html", "<!DOCTYPE html><html><head></head><body>" . $addtext . "</body></html>");
  if ($za->close()) {
  set_status(10);

    $za2 = new ZipArchive;
    $za2->open("./pp2/tmp.zip",ZipArchive::FL_RECOMPRESS);
    $za2->close();
    $file2 = file_get_contents("./pp2/tmp.zip");
    set_status(11);

    header("Content-Type: application/zip");
    header("Content-Disposition: attachment; filename=\"" . $label . "_html5.zip\"");
    echo "$file2";
  } else {
     echo "<pre>$file3 ERRORS FOUND, ZIP COULDN'T BE UPDATED</pre>";
    set_status(11);
  }
 } else {
     echo "<pre>$file3 ERRORS FOUND, ZIP FILE TOO LARGE</pre>"; 
 }

} else {
  echo "<pre>$file3";
  echo "THERE SEEMS TO BE ERRORS!</pre>";
    set_status(11);
}
