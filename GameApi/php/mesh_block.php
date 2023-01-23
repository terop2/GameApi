<?php

require_once("user.php");
$arr = choose_user();
if ($arr["user"] != "terop_priv" && $arr["user"]!="henri_priv") { die(); }
page_header(8);
page_title("block mesh","");
$user="terop";
echo "<form action=\"mesh_block_action.php\" enctype=\"multipart/form-data\" method=\"post\">";
echo "<input type=\"text\" name=\"idelem\" value=\"\">";
echo "<select name=\"block\">";
echo "<option value=\"block\">Block</option>";
echo "<option value=\"none\">None</option>";
echo "</select>";
echo "<input type=\"submit\" value=\"Submit\">";
echo "</form>";


echo "List of blocked meshes:<br>";
$user = "terop";
$num = read_num( $user );

for($i=$num;$i>1;$i--)
{
   $arr = array("username" => $user,
   	        "index" => $i);
   $val = read_url_block($arr);
   if ($val=="block") {
     echo create_id( $arr ) . "<br>";
   }
}


page_footer();