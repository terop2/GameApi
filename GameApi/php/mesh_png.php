<?php

$num = rand(0,1);

if ($num==0) {
   $data = file_get_contents( "http://meshpage.org/assets/torus.png" );
} else {
   $data = file_get_contents( "http://meshpage.org/assets/tunnel.png" );
}
echo "$data";