<?php


$private_key = openssl_pkey_new();
$public_key_pem = openssl_pkey_get_details($private_key)['key'];
echo $public_key_pem;
$public_key = openssl_pkey_get_public($public_key_pem);
var_dump($public_key);



$certstring = "file:///home/terop/meshpage.org.2.crt";
$cert = openssl_x509_read($certstring);

$private_key_string = "file:///home/terop/meshpage.org.2.key";
$private_key = openssl_x509_read($private_key_string);


$options = array("friendlyname"=>"");
$passphase = "";
$output = "";
$success = openssl_pkcs12_export( $cert, $output, $private_key, $passphrase, $options);

var_dump($output);
