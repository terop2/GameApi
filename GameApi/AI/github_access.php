<?php

$url = $_GET["loc"];

$file = file_get_contents("https://github.com/" . $url);
echo "$file";