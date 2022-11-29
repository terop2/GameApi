<?php

$filename = $_GET["filename"];
$file = file_get_contents($filename);
echo "$file";