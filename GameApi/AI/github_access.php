<?php

$url = $_GET["loc"];

$file = file_get_contents("https://raw.githubusercontent.com/" + $url);
echo "$file";