<?php

require_once("user.php");

$num = read_num( "terop" );
//for($i=$num-1;$i>=0;$i--)
//{
$i = $num-1;
   $arr = array("username" => "terop", "width"=>400, "height"=>300, "index" => $i+1, "id" => "400x300");
   //if (compare_files($arr)) {
     // TODO, add unique id to user_frame so that the elements don't use the 
     // same names/ids
     user_frame($arr,$i,NULL);
     user_table($arr,$i);
   //}
