<?php

$machine=php_uname("n");
if ($machine=="terop-pc2") {
   $site = "https://meshpage.org";
   $assetsite = "https://meshpage.org/assets";
   $sitename = "meshpage.org";
   } else {
   $site = "https://dinoengine.com";
   $assetsite = "https://dinoengine.com/assetsite";
   $sitename = "dinoengine.com";
   }


$url = $_GET["url"];

$githubaccess = substr($url,0,22);
if ($githubaccess=="github_access.php?loc=")
{
   $url2 = "https://github.com/" . substr($url,22);

   $headers = get_headers($url2,1);
   $size = isset($headers['Content-Length']) ? $headers['Content-Length'] : 0; 
   if (is_array($size)) {
      $size = end($size);
   }
   echo "A" . $size . " " . "0";
   return;
}


$parse = parse_url($url);
$host = $parse['host'];
$mode = false;
$chunksize = 0;
if (stream_is_local($url)) {
  $size=filesize($url);
} else if ($host=="meshpage.org"||$host=="ssh.meshpage.org"||$host=="dinoengine.com") {
  $path = $parse['path'];
  if ($machine=="terop-pc2") {
     $filename = "/home/terop/meshpage.org" . $path;
  } else {
     $filename = "/home/terop/" . $path;
  }   
  $size = filesize($filename);
  $splitfilename = $filename . ".aa.br";
  $splitfilename2 = $filename . ".aa";
  $mode = file_exists($splitfilename);
  if ($mode==true) {
    $chunksize = filesize($splitfilename2);
    }
} else if ($host=="meshpage.org/assets"||$host=="ssh.meshpage.org/assets") {
  $path = $parse['path'];
  if ($machine=="terop-pc2") {
    $filename = "/home/terop/meshpage.org/assets" . $path;
  } else {
    $filename = "/home/terop" . $path;
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
  $size = isset($data['Content-Length'])?$data['Content-Length']:0;
   if (is_array($size)) {
      $size = end($size);
   }
   $mode=true;
}
$char = 'A';
if ($mode==true) {
  $char = 'B';
}

echo $char . $size . " " . $chunksize;
