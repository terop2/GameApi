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


function save_cookie_status( $ip_address, $num )
{
  if ($ip_address === null || $ip_address == "") return;
  $filename = "./user_data/cookies/cookiedb.txt";

  $contents = file_get_contents($filename);
  $new_contents = "";
  $done = false;
  //if (strpos($contents, $ip_address) !== false) {
     $contents_array = explode(PHP_EOL, $contents);
     foreach($contents_array as $record) {
        if (strpos($record, $ip_address) !== false) {
	   //var_dump("REPLACE");
	   $new_contents .= $ip_address . " " . $num . PHP_EOL;
	   $done = true;
	} else {
	   //var_dump("OLD");
	   $new_contents .= $record . PHP_EOL;
	}
     }
  //}
  if ($done===false) {
     $new_contents .= $ip_address . " " . $num . PHP_EOL;
  }
  file_put_contents($filename, $new_contents);
}
function get_cookie_status( $ip_address )
{
  if ($ip_address === null || $ip_address == "") return "-1";
  $filename = "./user_data/cookies/cookiedb.txt";
  $contents = file_get_contents($filename);
  if (strpos($contents, $ip_address) !== false) {
     $contents_array = explode(PHP_EOL, $contents);
     foreach($contents_array as $record) {
        if (strpos($record, $ip_address) !== false) {
	   $result_arr = explode(" ", $record);
	   //var_dump($record);
	   return $result_arr[1];
	 }
     }
   }
   return "-1";
}
function get_client_ip()
{
   $fields = array(
   	   'HTTP_CF_CONNECTING_IP',
	   'HTTP_X_SUCURI_CLIENTIP',
	   'HTTP_CLIENT_IP',
	   'HTTP_X_FORWARDED_FOR',
	   'HTTP_X_FORWARDED',
	   'HTTP_FORWARDED_FOR',
	   'HTTP_FORWARDED',
	   'REMOTE_ADDR'
	   );
   foreach($fields as $ip_field) {
      if (!empty($_SERVER[$ip_field])) {
        return $_SERVER[$ip_field];
	}
   }
   return null;
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
function page_header_plain( $pos )
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
   echo "Once in a while some new technology appears that is so groundbreaking that markets fail to embrace it even though it is clearly the future.<p>";
   echo "When you combine perfect execution with the groundbreaking technology then you know the future has arrived.";
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
  // OPENGL LOGO
  echo "<div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"100\" height=\"50\"/>";
  echo "</div>";

  // WEBGL LOGO
  echo "<div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"100\" height=\"50\"/>";
  echo "</div>";

  // GLTF logo
  echo "<div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"100\" height=\"50\"/>";
  echo "</div>";

  // EMSCRIPTEN LOGO
  echo "<div style=\"width:170px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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pS/xqU99imOOOQbDMHj66ae5++67s0JotV3ivPPO8w6cWCxGKpXKai+RSPDwww8zffp0otEoAwMDvPPOOxx22GFeVqefuLZv3853vvMdrrzySubMmeNJH1u2bOGll15izZo1fPnLX+byyy8HXLVD2x386te0adMIh8PYtu2tVywWy7tXCPdrSIFAwIvs3EMzCsdxDhNCjEsU+8wAHMehvGQ6wUDRGNxdIITyuQhdSUD/e0xOoPb8j/JdGIsJCATVlTPZvWtXxpA1vlCT0eccYC1wsZ6s008/nRUrVtDe3p7lR3/rrbf4xje+QVVVFYFAgOHhYQYHB0mlUp6Pu7+/n/b2drZu3crrr7/uLYD+b09PD8888wzTp09n9uzZ3kbPLYARi8V49tlns0TTrVu3ZhnolHLr9j333HMcfPDBzJ4923Ob6bZWrVpFeXm5l0/e39/P008/zWOPPeYRv23bLFq0iEMOOYRAIMDpp5/O3XffnXWaxONx7rjjDn7/+99TXFxMOp0mFosxPDzsEcull15KZ2cnTU1NPPHEE1khrVqSeOKJJ2hoaKC4uJiamhqmT58+qk0gkUiwffv2rLEopejt7eXpp59m5syZzJkzh+HhYc/Vmrvp33rrLd5++22vLp6O/MyNcbjkkku8IBu/wTN3Lz/44IM8++yzhMNhhoeHGRgY4MYbb+QTn/gE//3f/52VrwCwevVq1q5dS0VFBYZhEIvFPLegzviMxWJs3ryZDRs2sH37dq9vGs3NzTzzzDNemLBhGOzatctze/olxM2bN7N69WpmzZqVZQDNMIoe3BqEY2KfGYCUCiFM3yL4J06gsEGBISwvt9+9y0HiYGDl5PwrHJVGYGD4ihErFFKlM62a5DIBVww2SafSOI7EssaXMXwT/SxuocYKKSXz5s3j8ssv58Ybb8wKzQW8qC69efw/O3bsYN26ddxwww1s2bIlyyikF+mVV17hC1/4AosWLeLOO+8knU7T3NycdR+4J9gVV1zBUUcdxT333EMgEPACSfz3bd26lSuvvJLGxkbuuOOOvJNh/fr1rF+/3hNf/WG84PraFy5cyDXXXENRkVvq70tf+hKvvfYab775Zp5I2dPT4yXI+JNz3nrrLbZt28ZNN93EM888kxXMo9He3s51112HZbnfj1yyZAm33HJL3ju0n7ulpSVv/tauXctll13G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\" width=\"150\" height=\"50\"/>";
  echo "</div>";

  // webassembly logo
  echo "<div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMoAAAB8CAMAAAAfKN/yAAAArlBMVEX///9lT/D9/f1dRfAAAABhSvCwp/bV0fqto/aLfPPv7+/i4uLAwMBWO+9jTfD5+fnOzs4eHh6mpqZaWlpUVFQICAgZGRksLCzz8v5ZP+91dXVoaGjX19czMzNJSUlDQ0OwsLCZmZmBgYGKioo7OzvBuvj49/6jmPWGdvPt6/1NL+6RhPMlJSWonvbm4/yek/XJw/lwXfG3r/d+bfLd2ft3ZfGYi/RrVvE/Fu1GI+4AqoDFAAAHcElEQVR4nO2baXejuBKGWTuxsNnMYrOYNY7t0M7q5Pb//2O3ShIYt8l0z0zmHOij90NAoKUeqCoJTCTlj5GkSH+IBMoYJVDGKIEyRgmUMUqgjFECZYwSKGPUl6Hc7WYD2h37dY7Dde6+xoIvQ7ld6ANa3PTr3AzXuf0aC74ORZUHpL7067wM1/n2NRb8xyj6bnuust3pE0aR1V4g3H1SZSIo5jlabhbmpFFkU72lWex4qw6TTAcFWJ5n9/ez589IJoQCMLqq6p+BTAvlFxIoVxIo1xIoAmVIAuVaAkWgDEmgXEugCJQhCZRrCRSBMiSBci2BMgEU0zS7PflyzzTPJyeAgm/oqblmt9fumPwF/lRQnrar7TNYq78eV6u3Axp8v1pt71VZf96uUA/6NFDW+NL7HY7o77CzxReT9D34SQckVv+bOg0U9RF6vVvw31eUmSqbH2/ABEfa1+H7iaDor1t8cw9mvuG574DyjEdgK7c/6cnmJFDMxV6Sjh+mvKbnntYAx+rosxUMir8ffVcngSIfwI1WO10/0XM3srlGex94qDy+A8/bYhoo1PJ3df0dR5COJxPZ9q86jf6tfgC/2z+bk0Ch/vRNxRi/AV+b6QceKjqU3vQFWv+oTgLFlOHQk6qC4fdwBx7XGPWQ0swPCX8opqR3E0HRAeCo7iAm9CeAOjxClQdVVnGegYlyIbG0MAEUNiNijG9/wNSy/x8tmvICuFYzXV7j7HLSp4GCmer0grPLO/jWD4x6WL/QJC2rOuYD6dt6Eij6DABuAeB2fQIvw29dIFT0V8zCp91ud68wtgmgmKc9S14P+gLMf9myUMEFDVtOQpOtrE8Bha618Mq/0o8QgGR7Mi++rcA262mgMKvf+EQPex+m+XHxIRIuaKaAoj7Qw08LNl3SacQ8wd3Z7x6oJLoKmAKKyVaSuCiW2z19Btu7Az5Dqj8gjiT6/DV6FPmAkY1zh6njuh6/oOqeyEBrfJJ5WU8CBWdDScF8i/O9tF+Y8mF//iiMut3qMAkUFWdB9liPXxTCg5ipwvbIF8TmM3oYrl3GjyKri8VifbFn4rZ7qbSG0iTC/vclUK4kUK4FKObPginl6tjVyRGirIc+h/4drceGcrz5xzr+uvffkfj3gjFKoIxRAmWMEihjlEAZowTKb3R80a/13wxyOeI1Cok2c9wayzqhZtQ1gb+RR5XRUzXuxlXSaxxvzvaSwtE0J7ZZyYh9TQtLQgvzDVXNzuXZkvA2RpYlbC+C89mySnh/xibvOi43rLoSR1dXZ+iulH5BhwmdCre276B1Xtg4II32rqWNE2h+UHStia/ZXaeeX+V5kbm0lISZm+dVygyda2UFKlnJbZrWzqoJeAfepiiKKtMKZq0RuJ1piVbwXuZXZg+h5E6MnVRhmtIxghJNXTo5QdHefd8gxChD32gbFZs47joIqH2EmqJUIVZSiMKNMNi4DGXp8ctLNqXTokR41mrLhnNGAaPQAMvzrqweRCGNQ9D2OqaWesGcogTnC2H4Gm0WdMeIV82blqvIjHNvVpz1O+coXG4219q7lWgdCqVL+JW/QDFC9JScN/olilT7NvYQ536OrkpdYxil69LWEitth8wb+9yZUob9IeYa6ZXclDTMZaLY0C7uCnQ5gAJ+aEhKUw5YPYhCXcrVXLuJYOyGuhtZNrkB4g4WwG5SBXEbfBFc+tLjVlqpMz8bbDtL+zwIXH3qp+yIG1oFjT6i2T0UYlkkyUprAEVpYqlI+3f2L1FsfwMdaoax8QzJDWjwk2WagdKCodRZVjdB0ZJYWo4e0d6jxNPiooOZB1qUK11hiYoNjqLYDl78wiOdgy2zOI6XmdtmsAsUuMR2XQ2QDKMYaUgULbOkKLCtiA6FKBswYcNRUhytTis+nJsmimJtoq6L+TJtuvRm5VmTcvec+xXkp8IlHMWSluBPVlZIZxTPzd0iTl1lCIUsw3BwmhpEsaJmbmsVhldBUpozMFbOAQCxgpsk1dgoildHoI3WG8OIeOKkspfc0J9iBaxyw0TClNELe14zH0KB45flv0SRiqAoMOSJFifobNJ12FOjq2bJDMWbBGr6g1hR3SdrWgMvMhigoHOWniT9nMGkMB5EMQbmlM9R5mEU07m3Dl02Xw6jlA3N70rVsOPxsn/ri/oiJ3ufoEhlbfjzAZTUU/49CiwiampV4Xh8gEEH4+sB0vA4zP1eFgYTewXLKTnKzw6GC4CUXKAwBI31+u9QJAxa3NpNWhOOktaogPWWYtQHDVsS5d2VZqOXbgK5Oma+TiJM4nbGEyhEgI06h71E4gAHO6PE0DzJmw1rYTQVbdGO8TdRCo0lViPTuoVFQMXXYI7v+86mYN2H3dqupNccWoO8nDWMaCnmiXquMTFzCp/ee3rDO5SNjxXCkttuOKxFxC1N/h6KRbgbENJaSbhoK7bbnbLO7RR62oCL2M05JIGbcC7121qk38q6GKk1TCGXoylkMBeLR69RSqCMUQJljBIoY5RAGaMEyhglUMYogTJGCZQxSqCMUYDyx+j/VUKmYY6vPBUAAAAASUVORK5CYII=\" width=\"100\" height=\"50\"/>";
  echo "</div>";

  // C++ LOGO
  echo "<div style=\"width:70px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"50\" height=\"50\"/>";
  echo "</div>";
  // OAUTH LOGO
  echo "<div style=\"width:70px;float: left;\">";
  echo "<img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAIAAAD/gAIDAAAtWUlEQVR42t3dB5gW1fk28E0zMVGjRsQKCuyy1C10dmkiKr1joag0URRFQZCqKCAgCkhRLKBEFBXFRAWsoFgwWDEmdo1J7JoYo0lM8v9+7zwyLLsLAqJe+c71XnPNnPfMmee5z/20mXl3s/7vO2r//e9//5O0f//737YOtzHSmC+++OLfSdvGyG+6ZX0nGG3t23/961+ff/753//+908//fSzzz775z//uQ34vn3gsr41jEop9re//e31119//PHHV6xYcdVVV02dOnX06NFDhw7t37//CSec0KtXr549ex577LH9+vUbMmTI2LFjZ8+effvttz/11FMffPBByXlAj3TfDmpZ3wJM6T7WvPHGG2vXrl2yZAl0hg8fPnDgwL59+/bu3btPnz52TjzxROjYP+6447p27dq2bdvi4uL8/Pxq1aoddNBBFStWPPTQQx127979ggsuuOeee95///108uDa/zyzGNcf//hHJLr11lsXLFhwySWXTJs27bLLLrvyyitvvPHGlStXrlu37tlnn/3973//2muvvfXWW28nzSmop/PJJ59cvXr1ddddByCANm7c+IADDthtt91233337Oxs4C5dujRFLSz0fwasklTid1Bp/fr1q1atYm533HGHnUcfffSFF16AxUcffcQY/540Tuof//gHPfmp6AzXVoov9t977z0zzJ07Fx+BlZW0ww477NRTT33ssce+UciyviGY6Gy1X375ZZRBjaeffhpAb775JlU/TJpv7b/77rvvJQ1wBjzyyCPPPffcX/7yF986xYl/+tOfYPfXv/71k08+CRBLen3Uu+GGG/i4/fbbD2Tf+9732rRpY0lSd7ZrIcva5UhR6c9JA0GYEozo/M4774DGjq9YWRwaAxc73Bkv1rBhQ25Lz0svvdShQ4fmzZszMQoDC/WCgA59e++997Lu9Oq//e1vJ0+enJubG0QrKiq666674qtd6P6zdiFMlHnpxRefeeaZ3/3ud6wPUm9takAB0KuvvmoHWO8k7d2kBb8Y7PTp05s1ayYaOgRHly5djjjiiF/+8pe0BZY1gBR+udCwYcP23HNPEeCKK64wZyrJxx9/rKdWrVoBWfv27YGY2u93D1aaNFHvwQceWPPgg2zHPkLB682kgQkRJk6c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\" width=\"50\" height=\"50\"/>";
  echo "</div>";
  
  // WEB3 LOGO
  echo "<!--div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"100\" height=\"50\"/>";
  echo "</div-->";
  echo "<div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"100\" height=\"50\"/>";
  echo "</div>";
  echo "<div style=\"width:60px;float: left;\">";
  echo "<img src=\"data:image/jpeg;base64,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\" width=\"40\" height=\"50\"/>";
  echo "</div>"; 
  echo "<div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"100\" height=\"50\"/>";
  echo "</div>";

  echo "</div>";
  echo "<p><br>";
  // COPYRIGHT NOTICE
   echo "<div style=\"height:20px;\"></div>";
   echo "<p>";
   echo "<div style=\"position: relative;\">";
   echo "<div style=\"width: 100%; float:left;\">";
   echo "<div style=\"font-family: 'calibri', sans-serif\"><strong>3d engine &copy; 2013-2022 Tero Pulkkinen, All rights reserved.<br>";
   echo "meshpage.org&reg; is a registered trademark of Tero Pulkkinen in finland.</strong></div>";
   echo "</div>";
   echo "</div>";
   echo "<div style=\"height:80px;\"></div>";
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
  if (array_key_exists( "user", $_COOKIE )) {
    echo "OK";
    setrawcookie("user", false, 1);
  }
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
   //session_start();
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