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
  echo "<img src=\"https://upload.wikimedia.org/wikipedia/commons/thumb/4/47/Emscripten_logo.svg/2560px-Emscripten_logo.svg.png\" width=\"150\" height=\"50\" crossorigin/>";
  echo "</div>";

  // webassembly logo
  echo "<div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"100\" height=\"50\"/>";
  echo "</div>";

  // C++ LOGO
  echo "<div style=\"width:70px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"50\" height=\"50\"/>";
  echo "</div>";
  // OAUTH LOGO
  echo "<div style=\"width:70px;float: left;\">";
  echo "<img src=\"https://farm4.static.flickr.com/3549/3343062926_4e65c72b65_o.png\" width=\"50\" height=\"50\" crossorigin/>";
  echo "</div>";
  
  // WEB3 LOGO
  echo "<!--div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,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\" width=\"100\" height=\"50\"/>";
  echo "</div-->";
  echo "<div style=\"width:120px;float: left;\">";
  echo "<img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAANkAAAB6CAMAAAAF6AYEAAAA9lBMVEX///8hFwcAAAD/tQD/DjP/cAIeEwAYCgAcEAAJAAD/sgC6ubn29vbu7e1WVlUhFgMkGw8RAAC0s7Hg397/rgCnpaPCwsL/AAA7NzVHR0YmHBrJx8X/597/aQD/ZAB5eHchIB5KQz//ACX/dSz/3+H/LTz/ACz/WAAvLi1hYWD/9fX/+vP/ABnR0M/+vU3/5Lz/xWD+y3qbmpn+1Jr+2qf/tSH/8d3/uDP+9Of/2cn+yrj/tJj/6c7/lWH/dzj+vUL/xsn/srf+RUj+hIj/bXf/XGX/r4v/hVH/fkT/bxyOjIn+nqH+vKT/0tP/nnX/ooMvKB7+T1nDTszcAAAH60lEQVR4nO2aC1fiOBTH24iU1rZYCtgKWkBeBasjo7M64yii62pRR7//l9k8mjSFIoLucfHkf2Z8NDS9v+Tm3ptUSRISEhISEhISEhISEhISEhISEhISEhISEhISEhLiNLy66n22Df+FhqNGu92+/mwzPl7D/XYGauvmy01bBoNlMvtXn23JB+uqQcAyjb+/wqT1qmdQVYRyHU1Zpt1ajMzWP9Ym4/39VW//ubu4h7q4u+stSxa4jlt+tymcQscJ39VB72G9s7NO1anCAEK9cZEQYrhWzqoF7zIloQHI5cDgPT1U72Ou9fXdX0tGkLKVl2Ww8R5LEtJNS5Ytf+n7+/D/A0+2vidJzTaKIe39hRLaBpAhWWlpSyalPyuyrJjG/A/qKcuxf3SAvt3xZOu3EO06s9V+GS9kyYeTbebeRGYUCmV76uqBph3Cb2cdnmznAV5qPj4uGPE/iyyo2EFl4lr/UlvTDpA/PiX8cfepitt7C7F9HlnBniDrf9PW1taKxxIKIgl/3L1/enh4+HX3exG0z/NGY2PCG48QGERDPz/t7uze7+3t7O6Q2dshul3Akg8mMwI5K8s5txLMy/+6PUF/WMRga9oR/KX/56yPr57d3u3Gk9f5OdVPuV4fxI+yPceLUhghKxjlsO7UvQ3+afBTPjespbpZSEBUQr/umF6BS4YmABAMThqAovcO4I3xswPf8dJm9PRyLdLJeaKh+rATr7r7SX+sqJbFpU8XKKDLkSk+NMlSLBW4cWYzfPgplxlRALALLu8NXABURbEA2GaWVkBeZgJe9EF0I6ty7C7s1Ush+16kZMWjiabqBUOb8scQ2q8wK21kQPQwTCYrSmQO99SKmsjhDvyIVWc9ekChFFmVTk8AlBy5llMUENVYLrxRNel96HnZ7nTNc6pRMO3b6VTrb4a2V0006J4KH+ZQ9wgwWYkjw1DYqDxbc2Xsp4yshtZPNx4qbnZkpRt1HboOvr7pQDclHmi4KKQ49MYSItuejPeSdEynTNP6U41S74KFyYfXyCrTZHBh1PMAzVJ2M5hPFuDllId3qTniedFCMuzNLIqNAYsPbyQ7YVM2PWMIjYWR+8XILDfAH0MepvrzyZBrwjEwByXPsTAa9a+pqP82sr4WeaP2PQ1Mkn5RtE7CHeeR5ZSoCX1OBsY8MrsGO8irOFTaqARm4WIJst4ZsvVHRPZX6pShKMLV/m8nY2HTwC2FeWQl5LaqRzFRS21JsrOLTqezDtPU+QlLZqn6RYPI3QJkeYs9yVfZHuQVshBPLe3Ot9AE6kuR/SS5Ck0ELkGKyVzG6WwZsrgFZi1UQcwj8xALoNYP0OCo9jJk1b149ZwWaWmVqv7FHtEdv9DmkCkuI7Mx6BwyHZHlutT6EiYLliH7SePCzm9JQnN2MpNMqlIlypA0skJMFhtCyPQ5ZMj/FIfeVMBklWXIblkGvpAktIe5nE2WrrRMzZG5s8lYqYjJiIEGJqvTWpCQlWeQ6agGUZ4TZKwG+fNushCPFCUjNpNxKyPmVG+sKLHPRmSWuTiZZEKyLEhYQkuWhbyxSZXwxgHgs2mB+62ynZ2KIKQ6DGCLTIs/SUdkUaBfnExmZF4iYi0QQXqjFtGoyV8uJNYMycfEMNuBlqiJqB/Zj1ssM7I/wPVxuASZb/Epoq4gH2GbmrdH/WF0BN4eJS6XkWcpUaWOTab+YaAhZTNDMjUZAd1XUGyPJtqzYg9+lex5kgz7C03rAd40mXHrmzP1mJK9JC7bKEBBAB2zICNpdYhNzkYxWyf2GxyMQjwnRM5IWwzEPIvMURILlyxdOb+NF2zgEDt426pvqq6aL/Q0deJsziOFr+KHpmLhzQp1QBwoc7WCYRi2iV3O41tkRXVDr6tmueLwVTJSRpZ02F00QrhmzqrbnldXcSUNpg8S5lbEY3oCvtVMNhhkFw83iRb+wfJZ76aKnwacZ4BMyLHAJfl4OOBeRc0hxux2MJ8sGkNQrwNQIz2VVfJsEG1WQdqx/7xdDAVrZyabSgSNbZ1jd7FreB+SJTvPLHfaY3dV7p645bV1RoIV2qVnGYKX2KYCM+3sZ87Oc0Rfx0w6I3rithrb6HDrAC5CRp1Xa/wxlm2CXNwSD7WHIyg1EEWFPBsqw6S79DxL8x72Bqwc8FMPtV4/Lbih7wbbreb0vRUfAGBZKvwaJg+P7BBeRGdAAPjJEwojVFgLt6cqA1WN6y7dBRaIw53t0lsYu1Qw8TX4mOfBjNO6V054eiMKlmmkvrTQjZLnm35Ynj4UM8ohbPEK04eEuMWcbKn4PKgRmgOuTz3AnZX4x+j2wIPdhMHMU8iZp3LNcYO6Ilxlq/g6N3GSKl0/kuU2vH6hwQMFxtV8Bc+ffl/vN/bbo1F7v0EmDP6G/q3oX4RwbyweW5HzRQExczO+Go9fVvZPC/p/0bdMLBYSH7wh9f1ib5n+X4reDA63eLAGSmE/zs9n1F2roehtLsvLNND/ODopFg9mFF4rIuSL44QvtiDYGlqB2lSiWzU1W/yUoXLqJCpQioefbdv71Bs3tvYZHKztD1lNeZRSU66WHq9fRi0Y+Vut0ahHEx0qT1Y6ikTqNR+Hw0cc7r8YGadjSqZ9W3lvnBDdlmorHkGmdVjEs1b8clMmSeeXmqadrHo6S9f58eEXix5CQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkJCQkIrr38BjfTaWJ32FO8AAAAASUVORK5CYII=\" width=\"100\" height=\"50\"/>";
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