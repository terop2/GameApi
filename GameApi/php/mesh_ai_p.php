<?php
ob_end_clean();

$prompt = $_GET["prompt"];

//echo "$prompt";
$descriptorspec = array(
   0 => array("pipe", "r"),
   1 => array("pipe", "w"),
   2 => array("pipe", "w")
);
/*
$process = proc_open("(cd /home/terop/cvs/meshy/;/home/terop/cvs/fastflux/bin/python3 /home/terop/cvs/meshy/api_request.py \"$prompt\" 2>/dev/null >/dev/null)", $descriptorspec, $pipes, null, null, array('bypass_shell' => true));
stream_set_blocking($pipes[1], 0);
$status = proc_get_status($process);
while($status['running']) {
   usleep(100000);
   $status = proc_get_status($process);
   }
   */
$file = popen("(cd /home/terop/cvs/meshy/;/home/terop/cvs/fastflux/bin/python3 /home/terop/cvs/meshy/api_request.py \"$prompt\" 2>/dev/null >/dev/null)","r");
while (!feof($file)) {
 $res = fread($file, 80);
 echo "$res";
}
fclose($file);


//$file2 = fopen("/home/terop/meshpage.org/pp2/meshy_log.txt","a");
//while (!feof($file)) {
// $res = fread($file, 10);
// fwrite($file2,$res);
//}
//$status = pclose($file);
//fclose($file2);

//proc_close($process);

//$file3 = fopen("/home/terop/cvs/meshy/refined_model.glb","rb");
//while (!feof($file3)) {
//    echo fread($file3, 4096);
//    }
//fclose($file3);
