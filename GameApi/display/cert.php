<?php

//phpinfo();

$req_key = openssl_pkey_new();
$dn = array(
    "countryName" => "FI",
    "stateOrProvinceName" => "Tampere",
    "organisationName" => "meshpage.org"
    );

$success2 = openssl_pkey_export( $req_key, $out_key );

$req_csr = openssl_csr_new( $dn, $req_key );
//echo "REQ_CSR:";
//var_dump($req_csr);

$ca_cert = "file:///home/terop/meshpage.org.cacert";
$private_key = "file:///home/terop/meshpage.org.privkey";


$client_cert = openssl_csr_sign( $req_csr, $ca_cert, $private_key, 30, null, 0);
//echo "CLIENT_CERT:";
//var_dump($client_cert);

$certout = "";
$success = openssl_x509_export( $client_cert, $certout );
//echo "SUCCESS:";
//echo "$success";

//echo "CERT_OUT:";
//echo "$out_key";
//echo "$certout";

$outfile = "";
openssl_pkcs12_export( $certout, $outfile, $out_key, "");
header("Content-Description: File Transfer");
header('Content-Disposition: attachment; filename="client_cert.pfx"');
header("Content-type: application/x-pkcs12");
echo "$outfile";


//echo "ERRORS:";
//while(($e = openssl_error_string()) !== false) {
//   echo $e . "\n";
//}