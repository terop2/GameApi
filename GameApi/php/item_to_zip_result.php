<?php

require_once("user.php");

$itemnum = $_POST["itemnum"];
$itemid = $_POST["itemid"];

$arr = decode_id( $itemid );

$arr2 = array("username" => $arr["username"],
      	      "index" => intval($arr["index"]),
	      "num" => intval($arr["index"])+1
	      );

$code = user_code($arr2, 0);

file_put_contents("./pp2/tmp.txt", $code);
file_put_contents("./pp2/tmp.zip", "");

$homepage = "https://tpgames.org/";

$cmd = "(cd ./pp2;LD_LIBRARY_PATH=.. nohup ../deploytool --file ./tmp.txt -o ./tmp.zip --homepage " . $homepage . " > test.txt 2>&1)";

shell_exec($cmd);

$file3 = file_get_contents("./pp2/test.txt");

$res = str_contains($file3, "ALL OK");

if ($res) {
  $file2 = file_get_contents("./pp2/tmp.zip");
  header("Content-Type: application/zip");
  header("Content-Disposition: attachment; filename=\"deploy.zip\"");
  echo "$file2";
} else {
  echo "<pre>$file3";
  echo "THERE SEEMS TO BE ERRORS!</pre>";
}
