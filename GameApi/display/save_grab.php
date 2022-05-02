<?php

function unpack_hexdump($str) {
   $str = rtrim($str, " \n\r\t\v\0$");
   $len = strlen($str);
   $res ='';
   for($i = 0;$i < $len;$i+=2) {
      $ch = $str[$i];
      $ch2 = $str[$i+1];
      $res .= chr(hexdec($ch.$ch2));
   }
   return $res;
}


$id = $_POST["id"];
$num = $_POST["num"];
$start = $_POST["start"];
$end = $_POST["end"];
$size = $_POST["size"];
//echo "$Id $num" . PHP_EOL;

$file = fopen("php://input","r+");
$line = fgets($file);
//echo "$line" . PHP_EOL;
$p = explode("|",$line);
if ($id=="") $id = $p[0];
if ($num=="") $num = $p[1];
if ($start=="") $start = $p[2];
if ($end=="") $end = $p[3];
if ($size=="") $size=$p[4];
$num = rtrim($num, " \n\r\t\v\0$");
$filename = "user_data/user_terop/grab" . $id . "_" . $num . "_" . $start . "_" . $end . ".part";
$rest = fread($file, 100000000);
//echo "$rest" . PHP_EOL;
$unhex = unpack_hexdump($rest);
file_put_contents($filename, $unhex);

$si = $size + 0;
echo "SIZE=" . $si;
// check if all files arrived already
for($i=0;$i<$si;$i+=1024)
{
  echo "i=$i" . PHP_EOL;
  $val = $i + 1023;
  if ($val>$si) $val=$si-1;
  echo "val=$i" . PHP_EOL;
  $filename = "user_data/user_terop/grab" . $id . "_" . $num . "_" . strval($i) . "_" . strval($val) . ".part";
  echo "filename=$filename" . PHP_EOL;
  $sz = 0;
  if (file_exists($filename))
     $sz = filesize($filename);
  echo "sz=$sz" . PHP_EOL;
  $ss = $val - $i + 1;
  echo "ss=$ss" . PHP_EOL;
  echo "$sz @@ $ss" . PHP_EOL;
  if ($sz==$ss) { echo "success" . PHP_EOL; } else { echo "not all files available yet:" . $i . "_" . $val . PHP_EOL; return; }
}

echo "APPEND";

$resfilename = "user_data/user_terop/grab" . $id . "_" . $num . ".png";
$file = fopen($resfilename, "w");

// append all files to single .png file
for($i=0;$i<$si;$i+=1024) {
$val = $i + 1023;
if ($val>$si) $val=$si-1;
$filename = "user_data/user_terop/grab" . $id . "_" . $num . "_" . strval($i) . "_" . strval($val) . ".part";
$data = file_get_contents($filename);
//$data = substr($data,0,-1);
fwrite($file,$data);
}
fclose($file);

echo "DELETE";

// delete parts.
for($i=0;$i<$si;$i+=1024)
{
  $val = $i + 1023;
  if ($val>$si) $val=$si-1;
  $filename = "user_data/user_terop/grab" . $id . "_" . $num . "_" . strval($i) . "_" . strval($val) . ".part";
  unlink($filename);
}

echo "MARK";
$fname = "user_data/touch.txt";
$file = fopen($filename,"w");
fwrite($file,"touched");
fclose($file);

echo "All done.";