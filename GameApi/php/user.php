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
  // LOGOS
  echo "<div>";
  echo "<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/e/e9/Opengl-logo.svg/512px-Opengl-logo.svg.png?20210504153942" width="100" height="50"/>
  echo "<img src="data:image/png;base64,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" width="100" height="50"/>
  echo "</div>";


  // COPYRIGHT NOTICE
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
    $x = 'https://meshpage.org/';
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