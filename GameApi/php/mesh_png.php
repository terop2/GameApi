<?php

$num = rand(0,1);

if ($num==0) {
   $data = file_get_contents( "http://tpgames.org/torus.png" );
} else {
   $data = file_get_contents( "http://tpgames.org/tunnel.png" );
}
echo "$data";