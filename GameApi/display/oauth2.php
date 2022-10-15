<?php
session_start();

include '/home/terop/html/tpgames.org/oauth2/RandomInterface.php';
include '/home/terop/html/tpgames.org/oauth2/Random.php';
include '/home/terop/html/tpgames.org/oauth2/AccessToken.php';
//include '/home/terop/tpgames.org/oauth2/Exception/OauthException.php';
include '/home/terop/html/tpgames.org/oauth2/OAuth2Client.php';
include '/home/terop/html/tpgames.org/oauth2/Provider.php';
include '/home/terop/html/tpgames.org/oauth2/HttpClientInterface.php';
include '/home/terop/html/tpgames.org/oauth2/CurlHttpClient.php';

function redirect_post($url, array $data, array $headers = null) {
  $params = [
    'http' => [
      'method' => 'POST',
      'content' => http_build_query($data)
      ]
      ];
      if (!is_null($headers)) {
         $params['http']['header'] = '';
	 foreach ($headers as $k => $v) {
	    $params['http']['header'] .= "$k: $v\n";
	    }
	    }
     $ctx = stream_context_create($params);
     $fp = @fopen($url, 'rb', false, $ctx);
     if ($fp) {
        echo @stream_get_contents($fp);
	die();
	} else {
	 throw new Exception("Error loading '$url'");
	 }
	 }


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
//http_response_code(302);
header("Cross-Origin-Resource-Policy: cross-origin");
header("Location: " . $authorizationRequestUri);

//$arr = array('url' => $authorizationRequestUri);

//redirect_post($authorizationRequestUri, $arr);
