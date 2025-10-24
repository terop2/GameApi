<?php
$ip = trim(@file_get_contents("https://api.ipify.org"));
echo "$ip\n";