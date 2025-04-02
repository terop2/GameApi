<?php

header("Content-Type: image/jpg");

$file = file_get_contents("/home/terop/cvs/fastflux/test.jpg");
echo "$file";
