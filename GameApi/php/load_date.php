<?php

$url = $_GET["url"];
$headers = get_headers($url);
$okstr = $headers[0];
$v = strstr($okstr,"200");
$t = NULL;
if ($v=="200 OK") {
   foreach($headers as $k => $v) {
     $u = strstr($v,"Last-Modified");
     $u2 = strstr($u," ");
     if ($u2 != "") {
        $t = new DateTime($u2);
     }
   }
}
if ($t!==NULL) {
  $s = 0;
  $s += $t->format('H') * 3600;
  $s += $t->format('i') * 60;
  $s += $t->format('s');
  var_dump($s);
}