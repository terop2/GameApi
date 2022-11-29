<?php
$id = $_GET["id"];
$append = $_GET["append"];
$data = file_get_contents("php://input");
$dt = "";
foreach(str_split($data) as $char) {
   if ($char=="$") $dt .= " ";
   else $dt .= $char;
}


$filename = "./user_data/temp/tmp" . $id . ".txt";
if ($append=="1") {
  file_put_contents($filename, $dt, FILE_APPEND);
} else {
  file_put_contents($filename, $dt);
}
