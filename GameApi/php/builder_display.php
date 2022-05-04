<?php

require_once("user.php");
require_once("emscripten_frame.php");

$data = $_POST["code"];
$homepage = $_POST["homepage"];
if ($data=="") {
   $data = $_GET["code"];
   $homepage = $_GET["homepage"];
}
$arr = array("username" => "terop",
     "index" => 0,
     "width" => 800,
     "height" => 600,
     "id" => "800x600");


echo "<html>";
frame($arr, 800,600, $data, 0, $homepage, "","","");
echo "</html>";