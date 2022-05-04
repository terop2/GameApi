<?php

#require_once("compile/compile.php");
require_once("emscripten_frame.php");
#require_once("source_table.php");

function create_new_user( $username, $passwd )
{
   mkdir( "./user_data/user_" . $username);
}
function check_user( $username, $passwd )
{
}
function copy_compile_files( $username, $num, $id )
{
}

function read_num( $username )
{
   $filename = "./user_data/user_" . $username . "/count.txt";
   if (file_exists($filename)) {
      $data = file_get_contents($filename);
      $num = intval($data);
   } else { $num=1; }
  return $num;
}
function write_num( $username, $num )
{
   $filename = "./user_data/user_" . $username . "/count.txt";
   $num_str = strval( $num );
   file_put_contents( $filename, $num_str ); 
}
function write_browser_support_string( $arr, $browser_list )
{
  $filename = "./user_data/user_" . $arr["username"] . "/browserlist.txt";
  file_put_contents( $filename, json_encode( $browser_list ) );
}
function read_browser_support_string( $arr )
{
  $filename = "./user_data/user_" . $arr["username"] . "/browserlist.txt";
  $data = file_get_contents( $filename );
  return json_decode( $data, true );
}

function write_browser_support_string2( $status )
{
  $_SESSION["pthread"] = $status;
}
function read_browser_support_string2( )
{
  if (array_key_exists( "pthread", $_SESSION )) {
    return $_SESSION["pthread"];
  }
  return "fail";
}


function write_url_block( $arr, $status )
{
   $filename = "./user_data/user_" . $arr["username"] . "/blocked" . $arr["index"] . ".txt";
   file_put_contents( $filename, $status );
}
function read_url_block( $arr )
{
   $filename = "./user_data/user_" . $arr["username"] . "/blocked" . $arr["index"] . ".txt";
   $data="none";
   if (file_exists($filename)) {
     $data = file_get_contents($filename);
   }
   return $data;
}
function set_lowmem( $arr, $num )
{
  $filename = "./user_data/user_" . $arr["username"] . "/lowmem" . $arr["index"] . ".txt";
  $num_str = strval( $num );
  file_put_contents( $filename, $num_str );
}
function set_background( $arr, $bg )
{
  $filename = "./user_data/user_" . $arr["username"] . "/bg" . $arr["index"] . ".txt";
  file_put_contents( $filename, $bg );

}
function addtext_date( $arr )
{
  $filename = "./user_data/user_" . $arr["username"] . "/addtext" . $arr["index"] . ".txt";
  return filemtime($filename);
}
function set_addtext( $arr, $addtext )
{
  $filename = "./user_data/user_" . $arr["username"] . "/addtext" . $arr["index"] . ".txt";
  file_put_contents( $filename, $addtext );
}
function set_label( $arr, $label )
{
  $filename = "./user_data/user_" . $arr["username"] . "/label" . $arr["index"] . ".txt";
  file_put_contents( $filename, $label );
}
function get_background( $arr )
{
  $filename = "./user_data/user_" . $arr["username"] . "/bg" . $arr["index"] . ".txt";
  if (file_exists($filename)) {
     $data = file_get_contents($filename);
     $bg = $data;
   } else { $bg=""; }
   return $bg;
}
function get_addtext ( $arr )
{
  $filename = "./user_data/user_" . $arr["username"] . "/addtext" . $arr["index"] . ".txt";
  if (file_exists($filename)) {
     $data = file_get_contents($filename);
     $addtext = $data;
   } else { $addtext=""; }
   return $addtext;
}
function get_label( $arr )
{
  $filename = "./user_data/user_" . $arr["username"] . "/label" . $arr["index"] . ".txt";
   if (file_exists($filename)) {
      $data = file_get_contents($filename);
      $lab = $data;
   } else { $lab=""; }
   return $lab;
}
function is_lowmem( $arr )
{
  $filename = "./user_data/user_" . $arr["username"] . "/lowmem" . $arr["index"] . ".txt";
   if (file_exists($filename)) {
      $data = file_get_contents($filename);
      $num = intval($data);
   } else { $num=0; }
   return $num;
}

function set_hidden( $arr, $num )
{
  $filename = "./user_data/user_" . $arr["username"] . "/hidden" . $arr["index"] . ".txt";
  $num_str = strval( $num );
  file_put_contents( $filename, $num_str );
}
function is_hidden( $arr )
{
  $filename = "./user_data/user_" . $arr["username"] . "/hidden" . $arr["index"] . ".txt";
   if (file_exists($filename)) {
      $data = file_get_contents($filename);
      $num = intval($data);
   } else { $num=0; }
   return $num;
}
function increase_num( $username, $new_num )
{
   $num = read_num( $username );
   if ($new_num > $num )
        write_num( $username, $new_num);
}
function new_display_from_compile( $username, $id, $num )
{
   increase_num( $username, $num );
   copy_compile_files( $username, $num, $id );
}
function new_display( $arr )
{
   $filename = "user_data/user_" . $arr["username"] . "/display" . $arr["index"] . "_" . $arr["id"] . ".js";
   $output = "";
   //if (!file_exists($filename)) {
      $output = compile_data( dirname(__FILE__) . "/compile/", $arr["data"], $arr["num"], $arr["width"] , $arr["height"] );
      new_display_from_compile( $arr["username"], $arr["id"], $arr["index"] );
   //}
   return $output;
}
function user_code( $arr, $i )
{
   $filename = "user_data/user_" . $arr["username"] . "/text" . $arr["index"] . ".txt";
   $data = file_get_contents($filename);
   return $data;
}
function html_start()
{
  echo "<html>";
}
function html_end()
{
  echo "</body>";
  echo "</html>";
}
function viewport_meta( )
{
	echo '<meta name="verifyownership" content="e77986b70c2f57469a1bbea0b80ca188"/>';
	echo '<meta http-equiv="origin-trial" content="AvkuION9OjDj+c5KxD0L/wgqyzkqE1vqOyceYiQe5LanN5395ZBJ/xfUuZcw7Mu7JkWiEskFjKGghchsKVVBKw4AAABYeyJvcmlnaW4iOiJodHRwczovL21lc2hwYWdlLm9yZzo0NDMiLCJmZWF0dXJlIjoiV2ViQXNzZW1ibHlUaHJlYWRzIiwiZXhwaXJ5IjoxNTYzOTI2Mzk5fQ==">';
	echo '<meta name="viewport" content="width=device-width, initial-scale=1.0">';
}
function twitter_meta( $arr, $j )
{
  $user = $arr["username"];
  $i = $arr["index"];
  $filename = "https://meshpage.org/user_data/user_" . $user . "/screenshot" . $i . ".webp";
  echo "<meta name=\"twitter:card\" content=\"summary\"/>";
  echo "<meta name=\"twitter:image\" content=\"" . $filename . "\"/>";
  $label = get_label($arr);

  echo "<meta name=\"twitter:title\" content=\"". $label . "\"/>";
  echo "<meta name=\"twitter:description\" content=\"Animated 3d model from meshpage.org\"/>";
}
function google_meta( $arr, $i )
{
  $user = $arr["username"];
  $i = $arr["index"];
  $filename = "https://meshpage.org/user_data/user_" . $user . "/screenshot" . $i . ".webp";
  $label = get_label($arr);
  echo '<meta property="og:title" content="' . $label . '"/>';
  echo '<meta property="og:description" content="3d animations"/>';
  echo '<meta property="og:image" content="' . $filename . '"/>';
}

function user_frame( $arr, $i, $noheaders, $seamless, $transparent )
{
   $filename = "user_data/user_" . $arr["username"] . "/text" . $arr["index"] . ".txt";
   $data = file_get_contents($filename);
   frame( $arr, $arr["width"], $arr["height"], $data, $i, $arr["homepage"], $noheaders, $seamless, $transparent );
}
function user_frame_clickable( $arr, $i, $noheaders, $seamless, $transparent )
{
  $id = $arr["id"];
  $width = $arr["width"] + 30;
  $height = $arr["height"]+50;
  $wd = $arr["width"];
  $hd = $arr["height"];
  $user = $arr["username"];
  $i = $arr["index"];
  $filename = "https://meshpage.org/user_data/user_" . $user . "/screenshot" . $i . ".png";
  $data="<div style=\"position:relative;\"><div style=\"position:absolute; top:0px; left:0px;\"><a href=\"https://www.meshpage.org/mesh_display?id=$id&noheaders=1&width=$wd&height=$hd&seamless=$seamless&transparent=$transparent\" target=\"game_frame\" onclick=\"hide()\"><img id=\"img\" src=\"$filename\" width=\"$width\" height=\"$height\"/></a></div><div><iframe name=\"game_frame\" width=\"$width\" height=\"$height\" style=\"margin:0px 0px 0px 0px; border-style: none;\"></iframe></div></div><script>function hide() { var o=document.getElementById(\"img\"); o.style.display='none'; }</script>";
  echo "$data";
}
function user_frame_embed( $arr, $i, $noheaders, $seamless, $transparent )
{
   $id = $arr["id"];

   $width = $arr["width"] +30;
   $height = $arr["height"] +50;
   $wd = $arr["width"];
   $hd = $arr["height"];
   $data = "<iframe allow=\"vr\" width=\"$width\" height=\"$height\" src=\"https://www.meshpage.org/mesh_display?id=$id&noheaders=1&width=$wd&height=$hd&seamless=$seamless&transparent=$transparent\"/>";
   echo "$data";
}
function user_table( $arr, $i )
{
   $filename = "user_data/user_" . $arr["username"] . "/text" . $arr["index"] . ".txt";
   $data = file_get_contents($filename);
   source_table( $data );
}
function save_data( $arr ) {
  $filename1 = "user_data/user_" . $arr["username"] . "/text" . $arr["index"] . ".txt";
  file_put_contents( $filename1, $arr["data"] );
  $filename2 = "user_data/user_" . $arr["username"] . "/num" . $arr["index"] . ".txt";
  file_put_contents( $filename2, $arr["num"] );
}
function save_homepage( $arr ) {
  $filename = "user_data/user_" . $arr["username"] . "/home" . $arr["index"] . ".txt";
  $data = $arr["homepage"];
  file_put_contents( $filename, $data );
}
function load_homepage( $arr ) {
   $filename = "user_data/user_" . $arr["username"] . "/home" . $arr["index"] . ".txt";
   $data = file_get_contents( $filename );
   return $data;
}
function save_screenshot( $arr )
{
  $filename1 = "user_data/user_" . $arr["username"] . "/screenshot" . $arr["index"] . "_" . $arr["screenshot_id"] . ".ppm";
  file_put_contents( $filename1, $arr["data"] );

}
function create_image( $filename, $width, $height, $newfilename )
{
   //list($ww,$hh) = getimagesize($filename); 
   //$img = imagecreatefrompng($filename);
   //$img2 = imagecreatetruecolor($width,$height);
   //imagecopysampled($img2,$img,0,0,0,0,$width,$height, $ww,$hh);
   //imagewebp($img2, $newfilename, 100);
   exec("convert $filename -resize " . $width . "x" . $height . " " . $newfilename);
}

function compare_files( $arr )
{
  $filename1 = "user_data/user_" . $arr["username"] . "/text" . $arr["index"] . ".txt";
  if (!file_exists($filename1)) { return false; }
  $filename2 = "user_data/user_" . $arr["username"] . "/display" . $arr["index"] . "_" . $arr["id"] . ".data";
  $filename3 = "user_data/user_" . $arr["username"] . "/display" . $arr["index"] . "_" . $arr["id"] . ".js";
  if (!file_exists($filename2)) { return false; }
  if (!file_exists($filename3)) { return false; }
  return true;
}
function compare_data( $arr )
{
  $filename1 = "user_data/user_" . $arr["username"] . "/text" . $arr["index"] . ".txt";
  if (!file_exists($filename1)) { return false; }
  $filename2 = "user_data/user_" . $arr["username"] . "/display" . $arr["index"] . "_" . $arr["id"] . ".data";
  $filename3 = "user_data/user_" . $arr["username"] . "/display" . $arr["index"] . "_" . $arr["id"] . ".js";
  if (!file_exists($filename2)) { return false; }
  if (!file_exists($filename3)) { return false; }
  $data = file_get_contents( $filename1 );
  return $data == $arr["data"];
}
function load_data( $arr ) {
  $filename1 = "user_data/user_" . $arr["username"] . "/text" . $arr["index"] . ".txt";
  $arr["data"] = file_get_contents( $filename1 );
  $filename2 = "user_data/user_" . $arr["username"] . "/num" . $arr["index"] . ".txt";
  $arr["num"] = file_get_contents( $filename2 );
}

function create_id( $arr )
{
  $str = $arr["username"] . "@" . $arr["index"];
  $begin = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ@";
  $end   = "ZYXWVUTSRQPONMLKJIHGFEDCBA0987654321zyxwvutsrqponmlkjihgfedcba@";
  $begin_chars = str_split( $begin );

  $i = 0;
  $chars = str_split( $str );
  foreach ( $chars as $ch ) {
     $j = 0;
     foreach ( $begin_chars as $ch2 ) {
     	 if ($ch2 == $ch) {
	    $str[$i] = $end[$j];
	 }
       $j = $j + 1;
     }
     $i = $i + 1;
  }
  return $str;
}
function js_mobile()
{
  $ua = $_SERVER["HTTP_USER_AGENT"];
  $val = strpos($ua, "Mobile");
  if ($val !== FALSE) { 
    return "yes";
  }
 return "no";
}

function js_highmem()
{
  $ua = $_SERVER["HTTP_USER_AGENT"];
  $firefox = strpos($ua, "Firefox") ? true : false;
  $match2 = false;
  preg_match( "#Firefox/(.+?)\.#", $ua, $match2 );
  $firefox_version = $match2[1];

  if ($firefox && $firefox_version <=78) { return "no"; }
  return "yes";
}

function js_no_threads()
{
  $ua = $_SERVER["HTTP_USER_AGENT"];
  $chrome = strpos($ua, "Chrome") ? true : false;
  $firefox = strpos($ua, "Firefox") ? true : false;
  $edge = strpos($ua, "Edge") ? true : false;
  $match = false;
  preg_match( "#Chrome/(.+?)\.#", $ua, $match );
  $match2 = false;
  preg_match( "#Firefox/(.+?)\.#", $ua, $match2 );
  $firefox_version = $match2[1];
  $chrome_version = $match[1];
  if ($firefox && $firefox_version <=78) { return "yes"; }
  if ($firefox) { return "no"; }
  if ($chrome && $chrome_version >= 70) { return "no"; }
  if (read_browser_support_string2()=="ok") { return "no"; }
  if (read_browser_support_string2()=="fail") { return "yes"; }
  if ($edge) { return "yes"; }
  if (js_mobile()=="yes") { return "yes"; }
 return "yes";
}


function small_page_header_head( $arr2 )
{
  if (js_mobile()=="yes") {
    echo "<link rel=\"stylesheet\" type=\"text/css\" href=\"mesh_css_mobile.css\">";
  } else {
    echo "<link rel=\"stylesheet\" type=\"text/css\" href=\"mesh_css.css\">";
  }
  viewport_meta();

}
function highlight_start($pos, $pos2, $level)
{
  echo "<div itemprop=\"itemListElement\" itemscope itemtype=\"http://schema.org/ListItem\" class=\"link level" . $level . "\">";
  echo "<meta itemprop=\"position\" content=\"" . $pos2 . "\"/>";
  if ($pos==$pos2) {
   echo "<b>";
  }
}
function highlight_end($pos, $pos2)
{
  if ($pos==$pos2) {
   echo "</b>";
  }
  echo "</div>";
}

function analytics( )
{
}


function small_page_header_body( $arr2, $num )
{
  analytics();
  $id = create_id( $arr2 );
  echo "<div itemscope itemtype=\"http://schema.org/BreadcrumbList\" id=\"navbar\" class=\"navi\">";
  echo "<div class=\"ncenter\">";
  highlight_start($num, 0,1);
  echo "<a itemprop=\"item\" class=\"navi\" href=\"mesh_all\"><span itemprop=\"name\">meshpage.org</span></a>";
  highlight_end($num,0);
  highlight_start($num, 1,1);
  echo "<a itemprop=\"item\" class=\"navi\" href=\"mesh_display?id=" . $id . "\"><span itemprop=\"name\">mesh display</span></a>";
  highlight_end($num,1);
  highlight_start($num, 2,2);
  echo "<a itemprop=\"item\" class=\"navi\" href=\"mesh_code?id=" . $id . "\"><span itemprop=\"name\">functions</span></a>";
  highlight_end($num,2);
  highlight_start($num, 3,2);
  echo "<a itemprop=\"item\" class=\"navi\" href=\"mesh_api?id=" . $id . "\"><span itemprop=\"name\">source code</span></a>";
  highlight_end($num,3);
  highlight_start($num, 4,2);
  echo "<a itemprop=\"item\" class=\"navi right\" href=\"mesh_html?id=" . $id . "\"><span itemprop=\"name\">html embed</span></a>";
  highlight_end($num,4);
  echo "</div>";  
  echo "</div>"; 
  //echo "<div class=\"separator\"></div>"; 
   echo "<div class=\"main\">";
}
function page_header( $pos )
{
   echo '<!DOCTYPE html>';
   echo "<html>";
   echo "<head>";
   echo "<title>meshpage.org -- create 3d animations</title>";
   echo "<meta name=\"description\" content=\"Meshpage is a mechanism to create 3d animations for your webpage\"/>";
   echo "<link rel=\"stylesheet\" type=\"text/css\" href=\"mesh_css.css\"/>";
   viewport_meta();
   echo "<meta name=\"robots\" content=\"noindex\">";
   echo "</head>";
   echo "<body>";
   analytics();
   echo "<div itemscope itemtype=\"http://schema.org/BreadcrumbList\" id=\"navbar\" class=\"navi\">";   
   echo "<div class=\"ncenter\">";
   highlight_start($pos, 0,1);
   echo "<a itemprop=\"item\" class=\"navi\" href=\"mesh_all\"><span itemprop=\"name\">meshpage.org</span></a>";
   highlight_end($pos,0);
   //echo " || ";
   highlight_start($pos, 1,1);
   echo "<a itemprop=\"item\" class=\"navi\" href=\"create_mesh\"><span itemprop=\"name\">create mesh</span></a>";
   highlight_end($pos, 1);

   //echo " | ";
   highlight_start($pos, 2,2);
   echo "<a itemprop=\"item\"  class=\"navi\" href=\"mesh_tool\"><span itemprop=\"name\">tool download</span></a>";
   highlight_end($pos, 2);

   //echo " | ";
   highlight_start($pos, 3,2);
   echo "<a itemprop=\"item\" class=\"navi\" href=\"mesh_faq\"><span itemprop=\"name\">faq</span></a>";
   highlight_end($pos,3);
   //echo " | ";
   highlight_start($pos, 4,2);
   echo "<a itemprop=\"item\" class=\"navi\" href=\"mesh_tutorial\"><span itemprop=\"name\">docs</span></a>";
   highlight_end($pos, 4);
   //echo " | ";
   highlight_start($pos, 5,2);
   echo "<a itemprop=\"item\" class=\"navi\" href=\"mesh_about\"><span itemprop=\"name\">about</span></a>";
   highlight_end($pos, 5);

   $arr = choose_user();
   if ($arr["user"] == "terop_priv")
   {
   //echo " | ";
   highlight_start($pos,6,1);
   echo "<a itemprop=\"item\" class=\"navi right\" href=\"mesh_hide.php\"><span itemprop=\"name\">vault</span></a>";
   highlight_end($pos,6);
   //echo " | ";
   highlight_start($pos,8,2);
   echo "<a itemprop=\"item\" class=\"navi right\" href=\"mesh_block\"><span itemprop=\"name\">block</span></a>";
   highlight_end($pos,8);  

   highlight_start($pos,9,2);
   echo "<a itemprop=\"item\" class=\"navi right\" href=\"mesh_stats\"><span itemprop=\"name\">stats</span></a>";
   highlight_end($pos,9);  

   //echo " | ";
   highlight_start($pos,10,2);
   echo "<a itemprop=\"item\" class=\"navi right\" href=\"mesh_logout\"><span itemprop=\"name\">logout</span></a>";
   highlight_end($pos,10);
   } else {
   //echo " | ";
   highlight_start($pos, 7,1);
   if ($_SERVER["HTTP_HOST"]=="localhost") {
   echo "<a itemprop=\"item\" class=\"navi right\" href=\"http://localhost/mesh-page/mesh_login_page\"><span itemprop=\"name\">login</span></a>";
   } else {
   echo "<a itemprop=\"item\" class=\"navi right\" href=\"https://www.meshpage.org/mesh_login_page\"><span itemprop=\"name\">login</span></a>";
   }
   highlight_end($pos,7);
   }

   echo "</div>";
   echo "</div>";
  //echo "<div class=\"separator\"></div>"; 
   echo "<div class=\"main\">";
}
function page_title( $str, $desc )
{
echo '<div style="font-family: \'calibri\', sans-serif" class="page_title">';
echo '<div rel="title" class="title"><h1>' . $str;
if ($_SERVER["HTTP_HOST"]=="localhost") {
  echo "(localhost)";
}
echo '</h1></div>';
echo '</div>';
if ($desc!="") {
echo '<div style="font-family: \'calibri\', sans-serif" rel="description" class="desc">' . $desc . '</div>';
}
echo "<div class=\"page_content\">";

}
function page_footer2()
{
   echo "<div style=\"height:20px;\"></div>";
   echo "<div style=\"position: relative;\">";
   echo "<div style=\"width: 80%; \">";
   echo "<div style=\"font-family: 'calibri', sans-serif\"><strong>3d engine &copy; 2013-2022 Tero Pulkkinen, All rights reserved.<br>";
   echo "meshpage.org&reg; is a registered trademark of Tero Pulkkinen in finland.</strong></div>";
   echo "</div>";
   echo "</div>";
   //echo "<div style=\"height:80px;\"></div>";
}
function page_footer()
{
   page_footer2();
   echo "</div>";
   echo "</div>";
   echo "</body>";
}
function list_start( $str )
{
  echo $str;
  echo "<ul>";
}
function list_end()
{
  echo "</ul>";
}
function list_item( $url, $str )
{
  echo "<li><a target=\"_blank\" href=\"" . $url . "\">" . $str . "</a>";
}
function set_user( $user )
{
  setrawcookie("user", password_hash($user, PASSWORD_DEFAULT), time()+60*50*24*30 );
}
function unset_user( $user )
{
  setrawcookie("user", "" );
}
function choose_user()
{
  $user = "terop";
  if (array_key_exists( "user", $_COOKIE ))
  {
    $user = $_COOKIE["user"];
  }
  if (password_verify("terop_priv", $user))
  {
  return array("user" => "terop_priv");  
  }
  else
  {
  return array("user" => "terop");
  }
}
function js_id()
{
  $filename = "web_page.js";
  $tag = md5_file( $filename );
  return $tag;
}
function js_versions()
{
   $arr = array("Chrome/[0-9.]*",
		"Chromium/[0-9.]*",
		"Mozilla/[0-9.]*",
		"AppleWebKit/[0-9.]*",
		"Safari/[0-9.]*",
		"Edge/[0-9.]*",
		"Firefox/[0-9.]*");
   return $arr;
}
function js_special()
{
  $ua = $_SERVER["HTTP_USER_AGENT"];
  $val = strpos($ua, "Android");
  if ($val !== FALSE) { 
     $matches = array();
     $val2 = preg_match("%Chrome/[0-9.]*%",$ua,$matches);

     $match = $matches[0];
     $val3 = strpos($match, "/");
     $val4 = substr($match, $val3+1);
     $num = intval($val4);
     if ($num>=61) return "ok";
     return "fail"; 
  }
  return "unknown";
}
function js_version( $user_arr )
{
	$val = js_special();
	if ($val=="ok") return "";
        if ($val=="fail") return "_asmjs";
	
	$webasm = "ok";
	if (in_array("webasm", $_POST))
		$webasm = $_POST["webasm"];
	if ($webasm == "ok") return "";
        if ($webasm == "fail") return "_asmjs";
	
	$web_data = read_browser_support_string( $user_arr );
	$ua = $_SERVER["HTTP_USER_AGENT"];
        $info = $web_data[$ua];
	if ($info == "ok") return "";
        if ($info == "fail") return "_asmjs";

	return "_asmjs";
}
function js_etag( $arr )
{
  $filename = "web_page.js";
  $val = filemtime( $filename );
  $etag = md5_file( $filename );
  $etag2 = md5_file( "mesh.php" );
  $etag3 = md5_file( "user.php" );
  $etag_comb = $etag . $etag2 . $etag3;
  header("Last-Modified: " . gmdate("D, d M Y H:i:s", $val) . " GMT");
  header("Etag: $etag_comb");
  header("Cache-Control: no-cache");
  if (trim($_SERVER['HTTP_IF_NONE_MATCH']) == $etag_comb) {
    header("HTTP/1.1 304 Not Modified");
    exit(0);
  }
}
function redirect_back_key()
{
?><script type="text/javascript">
var keys= {};
window.addEventListener("popstate",
  function(e) {
    history.back();
    history.back();
    e.preventDefault();
  }
,false);
</script>
<?php
}
function prevent_browser_keys()
{
?><script type="text/javascript">
var keys = {};
window.addEventListener("keydown",
 function(e) {
    keys[e.keyCode] = true;
    switch(e.keyCode) {
      case 37: case 39: case 38: case 40:
      case 32: e.preventDefault(); break;
      default: break;
    }
 }, false);
window.addEventListener('keyup',
  function(e) {
    keys[e.keyCode] = false;
  },
false);

</script>
<?php
}
function redirect_post( $url, array $data, array $headers=null )
{
   $params = array(
       'http' => array(
          'method' => 'POST',
          'content' => http_build_query($data)
        )
   );
   if (!is_null($headers)) {
     $params['http']['header'] = '';
     foreach( $headers as $k => $v) {
       $params['http']['header'] .= "$k: $v\n";
     }
  }
  $ctx = stream_context_create($params);
    $x = 'https://www.meshpage.org/';
    $fp = @fopen( $x . $url, 'rb', false, $ctx);
    if ($fp) { 
       echo @stream_get_contents($fp);
       die();
       } else {
       throw new Exception("Error loading '$url', $php_errormsg");
       }
}
function js_session_start()
{
   session_start();
}

function webassembly_detect( $user_arr, $id, $page, $noheaders, $seamless, $transparent, $width, $height, $meshdata, $homepage, $label )
{
  if ($_SERVER["HTTP_HOST"]=="localhost") {
     $ua = $_SERVER["HTTP_USER_AGENT"];
     $web_data[$ua] = "ok";
     write_browser_support_string( $user_arr, $web_data );
     return;
  }
  if ($_POST["pthread"]=="ok" || $_POST["pthread"]=="fail") {
     //var_dump("Writing pthread..");
     //var_dump($_POST["pthread"]);
     write_browser_support_string2( $_POST["pthread"] );
  }
  if ($_POST["webasm"]=="ok" || $_POST["webasm"]=="fail") {
     $web_data = read_browser_support_string( $user_arr );
     $ua = $_SERVER["HTTP_USER_AGENT"];
     $web_data[$ua] = $_POST["webasm"];
     write_browser_support_string( $user_arr, $web_data );
     if (array_key_exists( "pthread", $_SESSION ))
       return;
  }

  $ua = $_SERVER["HTTP_USER_AGENT"];
  $web_data = read_browser_support_string( $user_arr );
  $info = $web_data[$ua];
  if (array_key_exists( "pthread", $_SESSION )) {
     if ($info=="ok") return;
      if ($info=="fail") return;  
      }
  $arr = array(
       	 'id' => $id,
       	 'next' => $page,
       	 'noheaders' => $noheaders,
	 'seamless' => $seamless,
	 'transparent' => $transparent,
         'width' => $width,
         'height' => $height,
         'meshdata' => $meshdata,
         'homepage' => $homepage,
	 'label' => $label
	 );
  redirect_post("mesh_detect.php", $arr);
  //header("Location: mesh_detect.php?id=$id&next=$page&noheaders=$noheaders&width=$width&height=$height&meshdata=$meshdata&homepage=$homepage");
}

function decode_id( $str )
{
  $begin = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ@";
  $end   = "ZYXWVUTSRQPONMLKJIHGFEDCBA0987654321zyxwvutsrqponmlkjihgfedcba@";
  $end_chars = str_split( $end );
  $split = -1;
  $i = 0;
  $str_chars = str_split( $str );
  foreach ( $str_chars as $ch ) {
     $j = 0;
     foreach ( $end_chars as $ch2 ) {
     	 if ($ch2 == $ch) {
	    $str[$i] = $begin[$j];
	 }
       $j = $j + 1;
     }
     if ($ch == "@") { $split = $i; }
     $i = $i + 1;
  }
  $username = "terop";
  $num = 0;
  if ($split != -1) {
     $username = substr( $str, 0, $split );
     $num = substr( $str, $split+1, strlen($str)-$split-1 );
  }
  $arr = array("username" => $username,
       	       "index" => $num,
	       "width" => 800,
	       "height" => 600,
	       "id" => "800x600");
  return $arr;
}

function visit_counter_inc( $label )
{
   $filename = "./user_data/visit_" . $label . ".txt";
   $num = 0;
   if (file_exists($filename)) {
      $data = file_get_contents($filename);
      $num = intval($data);
   }
   $num = $num + 1;
   $num_str = strval( $num );
   file_put_contents( $filename, $num_str );   
   return $num;
}
function visit_counter_get( $label )
{
  $filename = "./user_data/visit_" . $label . ".txt";
   $num = 0;
   if (file_exists($filename)) {
      $data = file_get_contents($filename);
      $num = intval($data);
   }
   return $num; 
}