<?php

require_once("user.php");

$data = $_GET["id"];
$noheaders = $_GET["noheaders"];
$width = $_GET["width"];
$height = $_GET["height"];
if ($noheaders=="") { $noheaders = $_POST["noheaders"]; }
if ($width=="") { $width = $_POST["width"]; }
if ($height=="") { $height = $_POST["height"]; }

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
  $width = 350;
  $height = 250;
}
$arr2 = array("id" => $arr["id"],
	      "username" => $user,
              "index" => intval($arr["index"]),
  	      "num" => intval($arr["index"])+1,
	      "width" => $width,
	      "height" => $height);
//webassembly_detect( $arr2, $data, "mesh_embed.php", $noheaders, $width, $height, "", "" );

$homepage = load_homepage( $arr2 );
$arr2["homepage"] = $homepage;
//js_etag( $arr2 );
echo "<html>";
echo "<head>";
viewport_meta();
twitter_meta( $arr2, 0);
google_meta( $arr2, 0);
small_page_header_head( $arr2);
echo "</head>";
echo "<body>";
if ($noheaders==NULL) {
small_page_header_body( $arr2,1 );
page_title("mesh display","");
}
user_frame_clickable( $arr2, 0, $noheaders );
prevent_browser_keys();
redirect_back_key();
if ($noheaders==NULL) {
page_footer();
}
echo "</html>";
