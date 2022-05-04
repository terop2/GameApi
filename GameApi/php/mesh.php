<?php

require_once("user.php");

visit_counter_inc("mesh");
$data = $_GET["id"];
$noheaders = NULL;
$seamless = NULL;
$transparent = NULL;
$width = NULL;
$height = NULL;
if (array_key_exists("noheaders", $_GET)) { $noheaders = $_GET["noheaders"]; }
if (array_key_exists("seamless", $_GET)) { $seamless = $_GET["seamless"]; }
if (array_key_exists("transparent", $_GET)) { $transparent = $_GET["transparent"]; }
if (array_key_exists("width", $_GET)) { $width = $_GET["width"]; }
if (array_key_exists("height", $_GET)) { $height = $_GET["height"]; }

if (array_key_exists("noheaders", $_POST)) { if ($noheaders=="") { $noheaders = $_POST["noheaders"]; } }
if (array_key_exists("seamless", $_POST)) { if ($seamless=="") { $seamless = $_POST["seamless"]; } }
if (array_key_exists("transparent", $_POST)) { if ($transparent=="") { $transparent= $_POST["transparent"]; } }
if (array_key_exists("width", $_POST)) { if ($width=="") { $width = $_POST["width"]; } }
if (array_key_exists("height", $_POST)) { if ($height=="") { $height = $_POST["height"]; } }

$arr = decode_id( $data );
$arr["id"] = $data;

if (read_url_block( $arr ) == "block") { die(); }

$user = $arr["username"];
$num = read_num( $user );
if ($width == NULL) {
  $width = 800;
}
if ($height == NULL) {
  $height = 600;
}
if (js_mobile()=="yes")
{
  $width = 280;
  $height = 200;
}
$arr2 = array("id" => $arr["id"],
	      "username" => $user,
              "index" => intval($arr["index"]),
  	      "num" => intval($arr["index"])+1,
	      "width" => $width,
	      "height" => $height);
$label = get_label($arr2);
js_session_start();
webassembly_detect( $arr2, $data, "mesh_display", $noheaders, $seamless, $transparent, $width, $height, "", "", "" );

$homepage = load_homepage( $arr2 );
$arr2["homepage"] = $homepage;
js_etag( $arr2 );
echo "<html>";
echo "<head>";
viewport_meta();
twitter_meta( $arr2, 0);
google_meta( $arr2, 0);
small_page_header_head( $arr2);
echo "</head>";
echo "<body>";
if ($noheaders=="0" || $noheaders==NULL) {
small_page_header_body( $arr2,1 );
page_title($label,"");
}
echo "<div class=\"emscripten\">";
if (js_mobile()=="yes") {
$transparent = 0;
} else {
$transparent = 1;
}
user_frame( $arr2, 0, $noheaders, $seamless, $transparent );
prevent_browser_keys();
redirect_back_key();
echo "</div>";
echo "Want to create animations like this? <a href=\"mesh_tool.php\">Click here</a>";
if ($noheaders=="0" || $noheaders==NULL) {
page_footer();
}
echo "</html>";
