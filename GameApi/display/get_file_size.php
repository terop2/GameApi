<?php

$url = $_GET["url"];

$parse = parse_url($url);
$host = $parse['host'];
$mode = false;
$chunksize = 0;
if ($host=="meshpage.org") {
  $path = $parse['path'];
  $filename = "/home/terop/html/meshpage.org" . $path;
  $size = filesize($filename);
  $splitfilename = $filename . ".aa.br";
  $mode = file_exists($splitfilename);
  $chunksize = filesize($splitfilename);
} else if ($host=="tpgames.org") {
  $path = $parse['path'];
  $filename = "/home/terop/html/tpgames.org" . $path;
  $size = filesize($filename);
  $splitfilename = $filename . ".aa.br";
  $mode = file_exists($splitfilename);
  $chunksize = filesize($splitfilename);
} else {
  $data = get_headers($url,true);
  $size = isset($data['Content-Length'])?(int) $data['Content-Length']:0;
}
$char = 'A';
if ($mode==true) {
  $char = 'B';
}

echo $char . $size . " " . $chunksize;
