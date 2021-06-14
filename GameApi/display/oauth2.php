<?php
session_start();

include '/home/terop/tpgames.org/oauth2/RandomInterface.php';
include '/home/terop/tpgames.org/oauth2/Random.php';
include '/home/terop/tpgames.org/oauth2/AccessToken.php';
include '/home/terop/tpgames.org/oauth2/Exception/OauthException.php';
include '/home/terop/tpgames.org/oauth2/OAuth2Client.php';
include '/home/terop/tpgames.org/oauth2/Provider.php';
include '/home/terop/tpgames.org/oauth2/HttpClientInterface.php';
include '/home/terop/tpgames.org/oauth2/CurlHttpClient.php';

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
$authorizationRequestUri = $client->getAuthorizationRequestUri(
  'default-scope',
  'https://meshpage.org/oauth2_callback.php'
  );
$_SESSION['oauth2_session'] = $authorizationRequestUri;
http_response_code(302);
#echo "<meta http-equiv=\"refresh\" content=\"0; url=" . $authorizationRequestUri . "\" />";
header("Location: " . $authorizationRequestUri);

