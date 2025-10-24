<?php
$ip = $_GET["ip"];
$hostname=gethostbyaddr($ip);
echo "$hostname";
