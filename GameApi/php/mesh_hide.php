<?php
$machine=php_uname("n");
if ($machine=="terop-pc2") {
   $site = "https://meshpage.org";
   $assetsite = "https://meshpage.org/assets";
   $sitename = "meshpage.org";
   } else {
   $site = "https://dinoengine.com";
   $assetsite = "https://dinoengine.com/assetsite";
   $sitename = "dinoengine.com";
   }

require_once("user.php");
$pagestr = $_GET["id"];
if ($pagestr=="") $pagestr="0";
$page = intval($pagestr);

$arr = choose_user();
if ($arr["user"]!= "terop_priv" && $arr["user"]!="henri_priv") { die(); }

page_header_plain(6);
page_title("hidden vault","");

$user = "terop";
$num = read_num( $user );


echo "<h1>";
echo "<a href=\"mesh_hide.php?id=0\">1</a>";
if ($num>=50)
echo "<a href=\"mesh_hide.php?id=1\">2</a>";
if ($num>=100)
echo "<a href=\"mesh_hide.php?id=2\">3</a>";
if ($num>=150)
echo "<a href=\"mesh_hide.php?id=3\">4</a>";
if ($num>=200)
echo "<a href=\"mesh_hide.php?id=4\">5</a>";
if ($num>=250)
echo "<a href=\"mesh_hide.php?id=5\">6</a>";
if ($num>=300)
echo "<a href=\"mesh_hide.php?id=6\">7</a>";
if ($num>=350)
echo "<a href=\"mesh_hide.php?id=7\">8</a>";
if ($num>=400)
echo "<a href=\"mesh_hide.php?id=8\">9</a>";
if ($num>=450)
echo "<a href=\"mesh_hide.php?id=9\">A</a>";
if ($num>=500)
echo "<a href=\"mesh_hide.php?id=10\">B</a>";
if ($num>=550)
echo "<a href=\"mesh_hide.php?id=11\">C</a>";
echo "===>";
echo "123456789ABCDEFG"[$page];
echo "</h1>";


echo "<form action=\"mesh_hide_action.php\" enctype=\"multipart/form-data\" method=\"post\">";

$start = $page*50;
$end = $start+50;

if ($start>$num) $start=$num;
if ($end>$num) $end=$num;
if ($start<1) $start=1;
if ($end<1) $end=1;

for($i=$end;$i>$start;$i--)
{
  $arr = array("username" => $user,
               "index" => $i,
	       "width" => 200,
	       "height" => 150,
	       "id" => "200x150");
$id = create_id( $arr );
$label = get_label( $arr );
$addtext = get_addtext( $arr );
$filename = "user_data/user_" . $user . "/screenshot" . $i . ".png";
$filename2 = "user_data/user_" . $user . "/screenshot" . $i . ".webp";
if (file_exists($filename2)) $filename = $filename2;
echo "<p><div width=\"100%\"><div style=\"display: inline-block; *display: inline; vertical-align: top; width: 5%; height: 150px;\"><h1>$i</h1></div><div style=\"display: inline-block; *display: inline; vertical-align: top; width: 20%; height: 150px;\"> <a href=\"mesh.php?id=" . $id . "\"><img width=\"200\" height=\"150\" src=\"" . $filename . "\"/></a></div>";
echo "<div style=\"display: inline-block; *display: inline; width: 60%; height: 150px;  vertical-align: top;\"><div><a href=\"" . $site . "/" . $i . "\">LINK</a>";
echo "<input type=\"hidden\" name=\"" . $i . "_hide\" value=\"off\">";
echo "<input type=\"checkbox\" name=\"" . $i . "_hide\"";


if (is_hidden($arr)) {
echo " checked";
}
echo ">";
echo "<input type=\"hidden\" name=\"" . $i . "_lowmem\" value=\"off\">";
echo "<input type=\"checkbox\" name=\"" . $i . "_lowmem\"";
if (is_lowmem($arr)) {
echo " checked";
}
echo ">";
echo "<input type=\"text\" name=\"" . $i . "_label\" value=\"" . $label . "\">";
echo "<a href=\"mesh_code.php?id=" . $id . "\">Code</a>";
echo '<input type="file" name="png' . $i . '" id="filetoupload">';
echo "</div><div><textarea rows=\"4\" cols=\"80\" name=\"" . $i . "_addtext\"> " . $addtext . "</textarea>";
echo "</div></div></div><hr>";
}
echo "<input type=\"submit\" value=\"Submit\">";
echo "</form>";
page_footer();