<?php

require_once("user.php");
require_once("source_table.php");


$data = $_GET["id"];
$arr = decode_id( $data );
$user = $arr["username"];
$num = read_num( $user );
$arr2 = array("username" => $user,
              "index" => intval($arr["index"]),
	      "num" => intval($arr["index"])+1,
	      "width" => 800,
	      "height" => 600);
$code = user_code( $arr2, 0 );
echo "<html>";
echo "<head>";
small_page_header_head( $arr2 );
echo "</head>";
echo "<body>";
small_page_header_body( $arr2,2 );
page_title("functions","");
$i = $arr2["index"];
$filename = "user_data/user_" . $user . "/screenshot" . $i . ".png";
echo "<img width=\"200\" height=\"150\" src=\"" . $filename . "\"/>";
echo "<p>";
source_table( $code, $data );
echo "</body>";
echo "</html>";
