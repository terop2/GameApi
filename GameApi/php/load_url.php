<?php


$body = file_get_contents("php://input");
$p = explode("|",$body);

//var_dump($body);
//var_dump($p);

$url = $_POST["url"];
if ($url=="") $url=$p[0];
$homepage = $_POST["homepage"];
if ($homepage=="") $homepage=$p[1];

//var_dump($url);
//var_dump($homepage);

$domain = parse_url( $homepage );
$domain2 = parse_url( $url );
if (($domain['host'] == $domain2['host'] || $domain2['host']=="meshpage.org/assets") && $domain['host'] != "")
{
   $data = file_get_contents( $url );
   //$time = getlastmod();
    //$etag_comb = md5_file( $url );
    header("Cache-Control: max-age=86400");
    header("Cache-Control: no-store");
    $pack = true;
    if (empty($_SERVER["HTTP_ACCEPT_ENCODING"]) || strpos($_SERVER["HTTP_ACCEPT_ENCODING"], 'gzip') === false) { $pack = false; }
    $sz = strlen($data);
    if ($pack) {
       header("Content-Encoding: gzip");
       $data = gzencode($data, 9, FORCE_GZIP);
    }
    $size = strlen($data);
    header("Content-Length: $size");
    header("Uncompressed-Length: $sz");
   //header("Last-Modified: " . gmdate("D, d M Y H:i:s", $time) . " GMT");
   $etag_comb = md5( $url );
   header("Etag: $etag_comb");
   echo "$data";
} else {
   $v1 = $domain['host'];
   $v2 = $domain2['host'];
   echo "ERROR: $v1 != $v2";
}
