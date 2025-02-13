<?php

$prompt = $_GET["prompt"];

putenv('PATH=/usr/local/cuda/bin:/home/terop/cvs/fastflux/bin:/home/terop/cvs/fastflux/bin:/home/terop/cvs/fastflux/condabin:/home/terop/.pyenv/shims:/home/terop/.pyenv/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin:/home/terop/Android/Sdk/cmdline-tools/latest/bin:/home/terop/Android/Sdk/platform-tools:/home/terop/.local/bin:/home/terop/cvs/fastflux/pkgs/cudatoolkit-11.8.0-h4ba93d1_13/bin:' . getenv('PATH'));
putenv('LD_LIBRARY_PATH=/usr/local/cuda/lib64:/home/terop/cvs/fastflux/lib:/home/terop/cvs/fastflux/pkgs/cudatoolkit-11.8.0-h4ba93d1_13/lib:' . getenv('LD_LIBRARY_PATH'));

$res = "";
$file = popen("(cd /home/terop/cvs/fastflux/;/home/terop/cvs/fastflux/bin/python3 /home/terop/cvs/fastflux/terop.py $prompt 2>&1)", "rb");
while (!feof($file)) {
  $res .= fread($file, 4096);
}
$status = pclose($file);

if ($status=="120")
{
echo "EXIT STATUS: " . $status;
echo $res;
} else {
header("Content-Type: image/jpg");

$file = fopen("/home/terop/cvs/fastflux/test.jpg","rb");
while (!feof($file)) {
    echo fread($file, 4096);
}
}