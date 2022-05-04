<?php

require_once("user.php");

visit_counter_inc("mainpage");

$user = "terop";
$num = read_num( $user );


   page_header(0);
   page_title("meshpage.org","multithreaded crossplatform webgl animation technology");
   echo "<link rel=\"canonical\" href=\"https://meshpage.org/meshpage.php\"/>";
   echo "<div class=\"flex-container\">";

for($i=$num;$i>1;$i--)
{
$arr = array("username" => $user,
             "index" => $i,
	     "width" => 200,
	     "height" => 150,
	     "id" => "200x150");
if (is_hidden($arr)) continue;
if (js_mobile()=="yes" && !is_lowmem($arr)) continue;
if (read_url_block($arr)=="block") continue;
$id = create_id( $arr );
$label = get_label( $arr );

   $filename = "user_data/user_" . $user . "/screenshot" . $i . ".png";
   $filename2 = "user_data/user_" . $user . "/screenshot" . $i . ".webp";
   if (file_exists($filename2)) $filename = $filename2;
   echo "<div class=\"flex-item\" itemscope itemtype=\"http://schema.org/CreativeWork\">";
   echo "<div class=\"highlight\">";
   echo "<a class=\"label\" href=\"mesh_display?id=" . $id . "\" itemprop=\"url\">";
   echo "<div class=\"border\">";
   echo "<div class=\"image\">";
   echo "<img width=\"200\" height=\"150\" draggable=\"false\" src=\"" . $filename . "\" itemprop=\"thumbnailUrl\"/>";
   echo "</div>";
   echo "</div>";
   echo "<div class=\"label\" align=\"center\">$label</div>";
   echo "</a>";
   echo "</div>";
   echo "</div>";
}
   echo "</div>";
   page_footer();