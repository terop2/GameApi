<?php
session_start();
//header("Cross-origin-Resource-Policy: cross-origin");
//header("Access-Control-Allow-Origin: https://meshpage.org");
//header("Vary: Origin");
include '/home/terop/html/tpgames.org/oauth2/RandomInterface.php';
include '/home/terop/html/tpgames.org/oauth2/Random.php';
include '/home/terop/html/tpgames.org/oauth2/AccessToken.php';
include '/home/terop/html/tpgames.org/oauth2/Exception/OauthException.php';
include '/home/terop/html/tpgames.org/oauth2/OAuth2Client.php';
include '/home/terop/html/tpgames.org/oauth2/Provider.php';
include '/home/terop/html/tpgames.org/oauth2/HttpClientInterface.php';
include '/home/terop/html/tpgames.org/oauth2/CurlHttpClient.php';


$provider = new \fkooman\OAuth\Client\Provider(
  'meshpage',
  '',
  'https://auth.euderco.net/authorize',
  'https://auth.euderco.net/token'
  );
$client = new \fkooman\OAuth\Client\OAuth2Client(
  $provider,
  new \fkooman\OAuth\Client\CurlHttpClient()
  );

//echo "AUTH_CALLBACK2";
//var_dump($_SESSION);
//var_dump($_GET);
$auth = $_SESSION['oauth2_session'];
//echo "$auth";
$code = $_GET['code'];
$state = $_GET['state'];
//echo "$code";
//echo "$state";
$accessToken = $client->getAccessToken(
  $_SESSION['oauth2_session'],
  $_GET['code'],
  $_GET['state']
  );
//  echo "AFTER";
//var_dump($accessToken);
//unset($_SESSION['oauth2_session']);
//echo "TOKEN:\n";
//echo $accessToken->getToken() . "\r\n";
//echo "TOKENTYPE:\n";
//echo $accessToken->getTokenType() . "\r\n";
//echo "EXPIRES:\n";
//echo $accessToken->getExpiresIn() . "\r\n";
//echo "SCOPE:\n";
//echo $accessToken->getScope() . "\r\n";

//header("Authorisation: " . $accessToken->getToken() . " " . $accessToken->);
//header("Location: https://auth.euderco.net/userdata");
//http_response_code(302);


$opts = array(
      'http'=>array(
	'method'=>"GET",
	'header'=>"Authorisation: " . $accessToken->getTokenType() . "\r\n" .
		  "Access-Control-Allow-Origin: null\r\n"
	)
	);
$context = stream_context_create($opts);
$s = file_get_contents("https://auth.euderco.net/userdata?access_token=" . $accessToken->getToken(), false, $context);

$arr = json_decode($s,true);
echo "<!DOCTYPE html>";
echo "<div id=\"cnts\">" . $arr["user_id"] . "%";
echo "" . $arr["name"] . "%";
echo "" . $arr["email"] . "%";
echo "</div>";
//echo "<script>";
//echo "const frame = document.getElementById('frm');\n";
//echo "const cnts = document.getElementById('cnts');\n";
//echo "frame.contentWindow.postMessage(cnts, 'http://meshpage.org');";
//echo "</script>";