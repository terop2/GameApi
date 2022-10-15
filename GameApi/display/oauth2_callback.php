<?php
session_start();
include '/home/terop/html/tpgames.org/oauth2/RandomInterface.php';
include '/home/terop/html/tpgames.org/oauth2/Random.php';
include '/home/terop/html/tpgames.org/oauth2/AccessToken.php';
include '/home/terop/html/tpgames.org/oauth2/Exception/OAuthException.php';
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

$auth = $_SESSION['oauth2_session'];
$code = $_GET['code'];
$state = $_GET['state'];
$accessToken = $client->getAccessToken(
  $_SESSION['oauth2_session'],
  $_GET['code'],
  $_GET['state']
  );


$opts = array(
      'http'=>array(
	'method'=>"GET",
	'header'=>"Authorisation: " . $accessToken->getTokenType() . "\r\n" .
		  "Access-Control-Allow-Origin: null\r\n"
	)
	);
$context = stream_context_create($opts);
$s = file_get_contents("https://auth.euderco.net/userdata?access_token=" . $accessToken->getToken(), false, $context);
error_log($s);
$arr = json_decode($s,true);
echo "<!DOCTYPE html>";
echo "<div id=\"cnts\">" . $arr["user_id"] . "%";
echo "" . $arr["name"] . "%";
echo "" . $arr["email"] . "%";
echo "</div>";
