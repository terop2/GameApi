<?php

header("Content-Type: image/jpg");

$file = fopen("/home/terop/cvs/fastflux/test.jpg","rb");
while (!feof($file)) {
    echo fread($file, 4096);
}
