<?php

$code = $_POST["code"];
$homepage = $_POST["homepage"];
$date = date("Y-m-d H:i:s");

$data0 = file_get_contents("gameapi_0.html");
$data1 = file_get_contents("gameapi_1.html");
$data2 = file_get_contents("gameapi_2.html");
$data3 = file_get_contents("gameapi_3.html");

echo "$data0 $homepage $data1 $code $data2 $date $data3";