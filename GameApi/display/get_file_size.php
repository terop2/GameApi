<?php

$machine=php_uname("n");
if ($machine=="terop-pc2") {
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
} else if ($host=="meshpage.org"||$host=="ssh.meshpage.org"||$host=="dinoengine.com") {
  $path = $parse['path'];
  if ($machine=="terop-pc2") {
     $filename = "/home/terop/html/meshpage.org" . $path;
  } else {
     $filename = "/home/terop/html" . $path;
  }   
  $size = filesize($filename);
  $splitfilename = $filename . ".aa.br";
  $splitfilename2 = $filename . ".aa";
  $mode = file_exists($splitfilename);
  if ($mode==true) {
    $chunksize = filesize($splitfilename2);
    }
} else if ($host=="tpgames.org"||$host=="ssh.tpgames.org") {
  $path = $parse['path'];
  if ($machine=="terop-pc2") {
    $filename = "/home/terop/html/tpgames.org" . $path;
  } else {
    $filename = "/home/terop/html" . $path;
  }
  $size = filesize($filename);
  $splitfilename = $filename . ".aa.br";
  $splitfilename2 = $filename . ".aa";
  $mode = file_exists($splitfilename);
  if ($mode==true) {
     $chunksize = filesize($splitfilename2);
     }
  else
     $chunksize = 0;
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
