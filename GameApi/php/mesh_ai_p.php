<?php
ini_set('zlib.output_compression', 'Off');
ini_set('output_buffering', 'Off');
set_time_limit(0);
ob_end_clean();
ob_implicit_flush(true);

// Inform the browser we're using chunked encoding
// Make sure content type is set
header('Content-Type: text/event-stream');
// Disable caching to ensure real-time updates
header('Cache-Control: no-cache, no-store, must-revalidate');
header('Pragma: no-cache');
header('Expires: 0');
header('Accept-Encoding:');
header('X-Accel-Buffering: no');

function sendChunk($data) {
    echo $data;
    flush();
}

$prompt = $_GET["prompt"];

//echo "$prompt";

$file = popen("(cd /home/terop/cvs/meshy/;/home/terop/cvs/meshy/api_request2.sh '$prompt' 2>&1)","r");
stream_set_blocking($file,0);
while (!feof($file)) {
 $res = fgets($file);
 sendChunk("$res");
}
pclose($file);


