<?php

require_once("user.php");

function save_form()
{
?>
<form action="save_form.php" method="post">
<button type="submit" name="Save">Save</button>
</form>
<?php
}

function find_data()
{
   $data = file_get_contents("meshdata.txt");

   $len = strlen( $data );
   $maxval = 0;
   for($i = 0;$i < $len; $i++) 
   {
      if ($data[$i]=='I') {
         $var = substr( $data, $i+1 );
         $val = intval( $var );
         if ($val > $maxval) { $maxval = $val; }
      }
   }
  return $maxval;
}
function find_emscripten( $arr, $step )
{
  save_data($arr);
  $output = new_display($arr);
  return $output;
}
function send_message( $id, $message, $progress )
{
  $d = array('message' => $message, 'progress' => $progress );
  echo "id: $id" . PHP_EOL;
  echo "data: " . json_encode($d) . PHP_EOL;
  echo PHP_EOL;
  ob_flush();
  flush();
}
$meshdata = file_get_contents("meshdata.txt");
header( 'Content-type: text/event-stream' );
header( 'Cache-Control: no-cache');
send_message(1, "", 10);
$arr1 = array("data" => $meshdata,
             "num" => find_data(),
	     "username" => "terop",
	     "width" => 800,
	     "height" => 600,
	     "id" => "800x600",
	     "index" => read_num("terop"));
$msg1 = find_emscripten( $arr1, "1" );
send_message(2, "$msg1", 30);
$arr2 = array("data" => $meshdata,
             "num" => find_data(),
	     "username" => "terop",
	     "width" => 400,
	     "height" => 300,
	     "id" => "400x300",
	     "index" => read_num("terop"));
$msg2 = find_emscripten( $arr2, "2" );
send_message(3, "$msg2", 60);
$arr3 = array("data" => $meshdata,
             "num" => find_data(),
	     "username" => "terop",
	     "width" => 200,
	     "height" => 150,
	     "id" => "200x150",
	     "index" => read_num("terop"));
$msg3 = find_emscripten( $arr3, "3" );
send_message(4, "$msg3", 90);
send_message(5, "", 100);
//user_frame( $arr3 );
//save_form();