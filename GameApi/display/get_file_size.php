<?php

$machine=php_uname("n");
if ($machine=="terop-pc") {
   $site = "https://meshpage.org";
   $assetsite = "https://tpgames.org";
   $sitename = "meshpage.org";
   } else {
   $site = "https://dinoengine.com";
   $assetsite = "https://dinoengine.com/assetsite";
   $sitename = "dinoengine.com";
   }


$url = $_GET["url"];

$parse = parse_url($url);
$host = $parse['host'];
$mode = false;
$chunksize = 0;
if (stream_is_local($url)) {
  $size=filesize($url);
} else if ($host=="meshpage.org"||$host=="dinoengine.com") {
  $path = $parse['path'];
  if ($machine=="terop-pc") {
     $filename = "/home/terop/html/meshpage.org" . $path;
  } else {
     $filename = "/home/terop/html" . $path;
  }   
  $size = filesize($filename);
  $splitfilename = $filename . ".aa.br";
  $splitfilename2 = $filename . ".aa";
  $mode = file_exists($splitfilename);
  $chunksize = filesize($splitfilename2);
} else if ($host=="tpgames.org") {
  $path = $parse['path'];
  if ($machine=="terop-pc") {
    $filename = "/home/terop/html/tpgames.org" . $path;
  } else {
    $filename = "/home/terop/html" . $path;
  }
  $size = filesize($filename);
  $splitfilename = $filename . ".aa.br";
  $splitfilename2 = $filename . ".aa";
  $mode = file_exists($splitfilename);
  if ($mode)
     $chunksize = filesize($splitfilename2);
  else
     $chunksize = filesize($filename);
} else {
  $data = get_headers($url,true);
  $size = isset($data['Content-Length'])?(int) $data['Content-Length']:0;
  $mode=true;
}
$char = 'A';
if ($mode==true) {
  $char = 'B';
}

echo $char . $size . " " . $chunksize;
