<?php

$prompt = $_GET["prompt"];

//echo "$prompt";

$res="";
$file = popen("(cd /home/terop/cvs/meshy/;/home/terop/cvs/fastflux/bin/python3 /home/terop/cvs/meshy/api_request.py $prompt 2>&1)", "rb");
while (!feof($file)) {
 $res .= fread($file, 1024);
}
$status = pclose($file);


header("Content-Type: model/gltf-binary");

$file = fopen("/home/terop/cvs/meshy/refined_model.glb","rb");
while (!feof($file)) {
    echo fread($file, 4096);
fclose($file);
