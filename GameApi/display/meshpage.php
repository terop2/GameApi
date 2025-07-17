s<?php
header("Cross-Origin-Opener-Policy: same-origin");
header("Access-Control-Allow-Headers: Range");
//header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
//header("Cache-Control: post-check=0, pre-check=0", false);
//header("Pragma: no-cache");
$machine=php_uname("n");
$siteprefix = "";
if ($machine=="terop-pc2") {
   $site = "meshpage.org";
   $assetsite = "meshpage.org/assets";
   $sitename = "meshpage.org";
   $siteprefix=$_SERVER['HTTP_HOST'];
   $siteprefix=substr($siteprefix,0,4);
   //echo "SITEPREFIX: $siteprefix";
   if ("$siteprefix"!="ssh.") $siteprefix="";
} else {
   $site = "dinoengine.com";
   $assetsite = "dinoengine.com/assetsite";
   $sitename = "dinoengine.com";
}
   $site = "https://" . $siteprefix . $site;
   $assetsite = "https://" . $siteprefix . $assetsite;
//echo "SITE:" . $site;
//echo "ASSETSITE:" . $assetsite
?>
<!DOCTYPE html>
<html id="html"  style="background-color: #eee; overflow: auto;">
<head>
<title><?php echo $sitename ?> -- are you ready to bring the web to the next level technologies?</title>
<?php
$page = $_GET["p"];
if ($page=="") $page = $_GET["page"];
if ($page!="2") {
 echo '<meta name="description" content="meshpage.org makes the web 3d, one site at the time"/>';
 } else
 ?>
<meta http-equiv="origin-trial" content="AptK8NwNEYWXkj+auQSC8THBYvgBloOO5LemnbbmXRjmKwP7tV1EmbhaDZ02jO/PGuID0wNcCOXwQtfkuWsnNAgAAABjeyJvcmlnaW4iOiJodHRwczovL21lc2hwYWdlLm9yZzo0NDMiLCJmZWF0dXJlIjoiVW5yZXN0cmljdGVkU2hhcmVkQXJyYXlCdWZmZXIiLCJleHBpcnkiOjE2Mzk1MjYzOTl9"/>
<meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, shrink-to-fit=no"/>
<meta name="verifyownership" content="e77986b70c2f57469a1bbea0b80ca188"/>
<meta http-equiv="origin-trial" content="AvkuION9OjDj+c5KxD0L/wgqyzkqE1vqOyceYiQe5LanN5395ZBJ/xfUuZcw7Mu7JkWiEskFjKGghchsKVVBKw4AAABYeyJvcmlnaW4iOiJodHRwczovL21lc2hwYWdlLm9yZzo0NDMiLCJmZWF0dXJlIjoiV2ViQXNzZW1ibHlUaHJlYWRzIiwiZXhwaXJ5IjoxNTYzOTI2Mzk5fQ=="/>
<?php
require_once("user.php");
function create_id2( $name, $index )
{
  $str = $name . "@" . $index;
  $begin = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ@";
  $end = "ZYXWVUTSRQPONMLKJIHGFEDCBA0987654321zyxwvutsrqponmlkjihgfedcba@";
  $begin_chars = str_split( $begin );
  $i = 0;
  $chars = str_split($str);
  foreach( $chars as $ch ) {
    $j = 0;
    foreach( $begin_chars as $ch2 ) {
       if ($ch2 == $ch) { $str[$i] = $end[$j]; }
       $j = $j + 1;
    }
    $i = $i + 1;
    }
    return $str;
}

$id = $_GET["id"];



$label = create_id2("terop", $id); //$_GET["label"];
$page = $_GET["p"];
if ($page=="") $page = $_GET["page"];

$arr = decode_id( $label );
$arr["id"] = $id;

$user = "terop";

$arr2 = array("id" => $arr["id"],
              "username" => $user,
	      "index" => intval($arr["index"]),
	      "num" => intval($arr["index"])+1,
	      "width" => 800,
	      "height" => 600);

if ($page=="2") {
   twitter_meta( $arr2, $arr2["index"]);
   google_meta( $arr2, $arr2["index"]);
}

echo "<link rel=\"stylesheet\" type=\"text/css\" href=\"mesh_css.css?" . filemtime("mesh_css.css") . "\" />";

?>

<?php
require_once("user.php");
$nothreads = js_no_threads();
$mobile = js_mobile();
$highmem = js_highmem();
?>
<link rel="preload" href="vue.js" as="script"/>
</head>
<script type="application/ld+json">{
  "@context": "http://schema.org",
  "@type": "BreadcrumbList",
   "itemListElement": [
   {
     "@type": "ListItem",
     "position": 1,
     "item": {
        "@id": "<?php echo $site ?>/meshpage_3",
	"name": "meshpage.org"
	}
  },
  {
     "@type": "ListItem",
     "position": 2,
     "item": {
        "@id": "<?php echo $site ?>/meshpage_4",
	"name": "tool download"
     }
  },
  {
     "@type": "ListItem",
     "position": 3,
     "item": {
        "@id": "<?php echo $site ?>/meshpage_5",
	"name": "faq"
     }
  },
  {
     "@type": "ListItem",
     "position": 4,
     "item": {
        "@id": "<?php echo $site ?>/meshpage_6",
	"name": "documents"
     }
  },
  {
     "@type": "ListItem",
     "position": 5,
     "item": {
        "@id": "<?php echo $site ?>/meshpage_7",
	"name": "about"
     }
  }
  ]
}
</script>
<body id="body" style="overflow:hidden">
<script src="vue.js"></script>
<script>
if (!crossOriginIsolated) {
    console.log("NOT CROSSORIGIN ISOLATED => running in lowmem/nothreads mode");
    console.log("Your web server needs the following configuration to get gameapi builder animations working:");
    console.log("Header set Access-Control-Allow-Origin https://meshpage.org");
console.log("Header set Cross-Origin-Embedder-Policy \"require-corp\"");
console.log("Header set Cross-Origin-Resource-Policy \"same-site\"");
console.log("Header set Cross-Origin-Opener-Policy \"same-origin\"");
console.log("Header set Access-Control-Allow-Headers \"Range\"");
console.log("NOTE: you should change https://meshpage.org to your own web hosting location");

}

</script>

<div id="result" style="display:none"></div>
<div id="result2" style="display:none"></div>
<div id="app">
<div id="navbar" class="navi">
<div class="ncenter noselect">
<a href="JavaScript:void(0);" onClick="resume_cookies()">
<template v-if="indicator[0]">
<svg width="10" height="10">
<rect width="10" height="10" style="fill: #ff0000; stroke-width:0; stroke: rgb(0,0,0);">
</svg>
</template>
<template v-else>
<svg width="10" height="10">
<rect width="10" height="10" style="fill: #00ff00; stroke-width:0; stroke: rgb(0,0,0);">
</svg>
</template>
<!--template v-if="isIndicator3_2">
<svg width="10" height="10">
<rect width="10" height="10" style="fill: #00ff00; stroke-width:0; stroke: rgb(0,0,0);">
</svg>
</template>
<template v-if="isIndicator3">
<svg width="10" height="10">
<rect width="10" height="10" style="fill: #ff8800; stroke-width:0; stroke: rgb(0,0,0);">
</svg>
</template>
<template v-if="isIndicatorNone">
<svg width="10" height="10">
<rect width="10" height="10" style="fill: #ff0000; stroke-width:0; stroke: rgb(0,0,0);">
</svg>
</template>
<template v-if="isIndicator2">
<svg width="10" height="10">
<rect width="10" height="10" style="fill: #ff0000; stroke-width:0; stroke: rgb(0,0,0);">
</svg>
</template>
<template v-else>
<svg width="10" height="10">
<rect width="10" height="10" style="fill: #00ff00; stroke-width:0; stroke: rgb(0,0,0);">
</svg>
</template-->
</a>	
<template v-for="bread in main_breadcrumb">
<template v-if="bread.link">
<div style="font-family: 'calibri', sans-serif" class="link level1 highlightedtab">
<meta itemprop="position" v-bind:content="bread.num"/>
<a class="navi highlightedtab link" v-on:click="bread_click($event)"><b><span v-html="bread.title"></span></b></a>
</template>
<template v-if="!bread.link">
<div style="font-family: 'calibri', sans-serif" class="link level1">
<meta itemprop="position" v-bind:content="bread.num"/>
<a class="navi link" v-on:click="bread_click($event)"><span><b>{{ bread.title }}</b></span></a>
</template>
</div>
</template>
<div style="font-family: 'calibri', sans-serif; width: 120px; text-align: right; float: right; margin: 0 10 0 0; display:none;" class="link level1" id="login_label">
<a class="navi link" v-on:click="login_click($event)"  id="login_button"><span><div id="login_info">Anonymous</div></span></a>
</div>
</div>
</div>
<div class="dropdown-window" id="dropdown" style="display:none">
<div class="dropdown-content">
<b>
<a v-on:click="login_menu_click($event, 'profile')">Profile</a>
<a v-on:click="login_menu_click($event, 'create_new')">Create new anim</a>
<a v-on:click="login_menu_click($event, 'my_animations')">My Animations</a><hr>
<a href="#">Logout</a>
</b>
</div>
</div>
<div class="main">
<div v-if="state.empty">
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
<br><br><br><br><br><br>
</div>
<?php
if ($sitename=="dinoengine.com") {
  echo "<div class=\"logo\">";
  echo "<img loading=\"lazy\" src=\"dino3.webp\" width=\"200\" height=\"150\"></img>";
  echo "</div>";
  }
?>
<div id="profile" style="display:none">
<div style="font-size: 20px;"><span id="log">#</span>(<span id="name">#</span> - <span id="email">#</span>)</div>
<form action="/update_profile.php" method="post">
<label for="nickname">Display name:</label>
<input type="text" id="nickname"><br><br>
<label for="homepage">Homepage:</label>
<input type="text" id="homepage"><br><br>
<input type="submit"><br><br>
</form>
</div>
<div v-if="state.create_new">
<div class="tab">
  <button class="tablinks active" onclick="open_tab(event,'pkg')">Package files</button>
  <button class="tablinks" onclick="open_tab(event,'script')">Script files</button>
</div>
<div id="pkg" class="tabcontent">
  <h3>Package files</h3>
  <form action="/submit_pkg.php" method="post">
<label for="screenshot">Screenshot(.png):</label>
<input type="file" id="screenshot">
<br><br>
  <p>Please enter urls for Game_*.pkg files:</p>
<label for="pkg_1">url for game_1.pkg:</label>
  <input type="text" id="pkg_1" ><br>
  <label for="pkg_2">url for game_2.pkg:</label>
  <input type="text" id="pkg_2" ><br>
  <label for="pkg_3">url for game_3.pkg:</label>
  <input type="text" id="pkg_3" ><br>
  <label for="pkg_4">url for game_4.pkg:</label>
  <input type="text" id="pkg_4" ><br>
  <label for="pkg_5">url for game_5.pkg:</label>
  <input type="text" id="pkg_5" ><br>
  <label for="pkg_6">url for game_6.pkg:</label>
  <input type="text" id="pkg_6" ><br>
  <input type="submit">
  </form>
</div>
<div id="script" class="tabcontent notdefault">
  <h3>Script file</h3>
  <form action="/submit_script.php" method="post">
<label for="screenshot">Screenshot(.png):</label>
<input type="file" id="screenshot">
<br><br>
  <p>Please enter the script</p>
  <label for="script">Script</label><br>
  <textarea id="script" name="script" rows="30" cols="80">
  </textarea><br>
  <input type="submit">
  </form>
</div>
<script>
function open_tab(event, label) {
  var tabcontent = document.getElementsByClassName("tabcontent");
  var i;
  for(i=0;i<tabcontent.length; i++) {
     tabcontent[i].style.display="none";
  }
  var tablinks = document.getElementsByClassName("tablinks");
  for(i=0;i<tablinks.length;i++) {
     tablinks[i].className = tablinks[i].className.replace(" active", "");
  }
  document.getElementById(label).style.display = "block";
  event.currentTarget.className += " active";
}
</script>
</div>
<div v-if="state.my_animations">
</div>
<div id="main_div">


<script>
/*
function populate_imgs()
{
   var s = imgarr.length;
   var i = 0;
   for(i=0;i<s;i++)
   {
      var node = imgarr[i];
      var tag = node.tag;
      var filename = node.filename;
      //console.log(tag);
      //console.log(filename);
      var tag2 = document.getElementById(tag);
      tag2.src = filename;
   }
}
*/
/*
function preload_anim(num, file_id)
{
  for(var i=0;i<15;i++)
  {
   var filename = "<?php echo $site ?>/user_data/user_terop/grab";
   var num2 = file_id.toString();
   var str = i.toFixed().toString();
   while (str.length<3) str="0".concat(str);
   var filename2 = filename.concat(num2).concat("_").concat(str).concat(".png").concat("?").concat(<?php echo '"' . filemtime("user_data/touch.txt") . '"'?>);
   var url = filename2;

   //const preloadImage = new Image();
   //preloadImage.src=url;
   var tag = document.getElementById("preload".concat(i.toString()));
   tag.src = url;
  }
}
*/
function load_anim_pic_reset(num,file_id)
{
   var name = "displayimage".concat(num.toString());
   var imgtag = document.getElementById(name);
   imgtag.onmousemove = function() { }

 if (file_id!=-1) { 
   imgtag.src="<?php echo $site ?>/user_data/user_terop/screenshot".concat(file_id.toString()).concat(".webp");
   }

}
</script>
<?php
require_once("user.php");
$user="terop";
$num = read_num( $user );
echo "3D ENGINE STATUS: <span id=\"engstatus\">WAITING FOR NECESSARY COOKIES..</span><br>";
echo "<div style=\"display:none\">LOGIN STATUS: <span id=\"loginstatus\">WAITING FOR COOKIES..</span></div><br>";
echo "<div style=\"display:none\">PURCHASE STATUS: <span id=\"status\">WAITING FOR COOKIES..</span></div>";
page_title($sitename, "groundbreaking way to bring the next level technologies to the web: 3d.");
echo "<div class=\"buttons\">";
echo "<button type=\"button\" class=\"butinner\" onclick=\"window.location='https://meshpage.org/view.php'\">3d model viewer</button>";
echo "<button type=\"button\" class=\"butinner\" onclick=\"window.location='https://meshpage.org/gltf_to_zip.php'\">Convert GLTF file to html5 zip</button>";
echo "</div>";
echo "<div class=\"flex-container\">";
$cnt = 0;
$start = $num;
$startpos = $_GET["ps"];
if ($startpos!="") {
  $start = $num-($startpos*50);
}
$dupcache = array();

echo "<script>var g_focus=false; var g_focus2 = false; var g_focus3 = false;\n";
echo "var g_focus4 = false;\n";
echo "function show_copy3(ii,dt) {\n";
echo " var d2 = document.getElementById(\"copybutton\" + ii);\n";
echo " if (d2==null) return;\n";
echo " dt = dt.trim();\n";
echo "   if (dt==\"\"||dt==\";\") {  d2.disabled=\"true\"; } else { d2.disabled=\"\"; }\n";
echo "}\n";
echo "function show_apk3(ii,dt) {\n";
echo " var d2 = document.getElementById(\"apkbutton\" + ii);\n";
echo " if (d2==null) return;\n";
echo " dt = dt.trim();\n";
//echo " console.log(dt);\n";
echo "   if (dt==\"0\"||dt==\"\") {  d2.disabled=\"true\"; } else { d2.disabled=\"\"; }\n";
echo "}\n";
echo "function show_apk(ii,ump) {\n";
echo " window.location.href=\"/mesh_apk.php?id=\" + ii.toString() + \"&full=true\";";
echo "}\n";
echo "function handleSubmit(ii) {\n";
echo "  e.preventDefault(); \n";
echo "  console.log(\"handlesubmit(\" + ii.toString + \")\");\n";
echo "}\n";
//echo "console.log(apk_status);\n";
echo "</script>";

$display_labels = array();

$iii=0;
$counter=0;
for($i=$start;$cnt<50;$i--)
{
$iii++;
$ii = $i;
if ($i<=1) {
  redo:
  $ii = rand(2, $num);
  foreach($dupcache as $i)
  {
    $counter=$counter+1;
    if ($counter>30) goto enough;
    if ($i==$ii) goto redo;
  }
  array_push($dupcache, $ii);
}
enough:
$arr = array("username" => $user,
             "index" => $ii,
	     "width" => 200,
	     "height" => 150,
	     "id" => "200x150");
if (is_hidden($arr)) continue;
if (js_mobile()=="yes" && !is_lowmem($arr)) continue;
if (read_url_block($arr)=="block") continue;


$cnt = $cnt + 1;
$id = create_id( $arr );
$label = get_label( $arr );

$display_labels[$ii] = $label;

   $filename = "<?php echo $site ?>/user_data/user_" . $user . "/screenshot" . $ii . ".png";
   $filename2 = "<?php echo $site ?>/user_data/user_" . $user . "/screenshot" . $ii . ".webp";
   $ik = $ii;
   if (file_exists($filename2)) $filename = $filename2;
   if (!file_exists($filename)) { $ik=-1; $filename = $assetsite . "/unknown.webp"; }



   $url = "/" . $ii; //"meshpage/2&id=" . $ii; // . "&label=" . $id;
   echo "<div class=\"flex-item\" itemscope itemtype=\"http://schema.org/CreativeWork\">";
   echo "<div class=\"highlight\">";
   echo "<div style=\"width: 100%; font-family: 'calibri', sans-serif\" class=\"label\" align=\"center\">$ii : $label</div>";
   echo "<a class=\"label\" href=\"$url\" v-on:click.prevent=\"mesh_display(" . $ii . ",'" . $id . "','" . $label ."')\" itemprop=\"url\">";
   echo "<div class=\"border\">";
   echo "<div class=\"image\">";
   // BACKGROUND CHANGE
   //echo "<layer width=\"200\" height=\"150\">";
   //echo "<div style=\"width: 200; height:150; background: rgba(0,0,0,1);\"></div>";
   //echo "</layer>";
   //echo "<layer width=\"200\" height=\"150\">";
   if (file_exists("user_data/user_terop/screenshot" . $ii . ".webp"))
   {
   echo "<img loading=\"lazy\" src=\"user_data/user_terop/screenshot" . $ii . ".webp\" id=\"displayimage" . $iii . "\" class=\"displayimage\" width=\"200\" height=\"150\" draggable=\"false\"   crossorigin/>";
   } else {
   echo "<img loading=\"lazy\" src=\"" . $assetsite . "/unknown.webp\" id=\"displayimage" . $iii . "\" class=\"displayimage\" width=\"200\" height=\"150\" draggable=\"false\"  crossorigin/>";
   }
   //echo "</layer>";
   // src=\"" . $filename . "\"
   //echo "<script>imgarr.push({ tag:\"displayimage" . $iii . "\", filename : \"" . $filename . "\"});</script>";




   // END OF BACKGROUND CHANGE
   echo "</div>";
   echo "</div>";
   $ump = create_id2("terop",$ii);

   echo "<div class=\"zipbutton\">";
   echo "<div class=\"ziphoriz\">";
   echo "<button id=\"copybutton$ii\" type=\"button\" onclick=\"show_copy($ii,'$ump')\" onfocus=\"g_focus3=true;\" onblur=\"g_focus3=false;\" style=\"height:21px\">&copy;</button>";
   echo "<script>";
//         $arr = array("username" => "terop", "index" => $_GET["id"]);
//      $res = addtext_date($arr);
//      echo "\nvar dt = \"$res;\";\n";
//echo "   fetch(\"https://meshpage.org/mesh_addtext.php?id=$ump&\" + dt )\n";
//echo "     .then(x => x.text())\n";
//echo "     .then(y => show_copy3($ii,y));\n";
echo "</script>";
echo "<button type=\"button\" onclick=\"show_script($ii,'$ump')\" onfocus=\"g_focus2=true;\" onblur=\"g_focus2=false;\" style=\"height:21px\">Script</button>";
   echo "<form id=\"form" . $ii . "\" method=\"GET\" action=\"/item_to_zip_result.php\">";
   echo "<input type=\"hidden\" name=\"itemnum\" value=\"" . $ii . "\">";
   echo "<input type=\"hidden\" name=\"itemid\" value=\"" . $id . "\">";
   echo "<input type=\"submit\" value=\"Zip\" onfocus=\"g_focus=true;\" style=\"height:21px\" onblur=\"g_focus=false;\">";
   echo "</form>";


   //echo "<form onsubmit=\"handleSubmit(" . $ii . ");\" id=\"formapk" . $ii . "\" method=\"GET\" action=\"/mesh_apk.php\">";
   //echo "<input type=\"hidden\" name=\"id\" value=\"" . $ii . "\"/>";
   //echo "<input type=\"hidden\" name=\"full\" value=\"true\"/>";
   //echo "<input id=\"apkbutton" . $ii ."\" type=\"submit\" value=\"Apk\" onfocus=\"window.g_focus4=true;\" onblur=\"window.g_focus4=false;\"/>";
   //echo "</form>";

echo "<button id=\"apkbutton$ii\" type=\"button\" style=\"height:21px\" onclick=\"show_apk($ii,'$ump')\" onfocus=\"g_focus4=true;\" onblur=\"g_focus4=false;\">Apk</button>";
   echo "<script>";
         $arr = array("username" => "terop", "index" => $_GET["id"]);
      $res = addtext_date($arr);
      echo "\nvar dt = \"$res;\";\n";

//echo "   fetch(\"https://meshpage.org/mesh_apk.php?id=$ii&full=false\")\n";
//echo "     .then(x => x.text())\n";
//echo "     .then(y => show_apk3($ii,y));\n";
//echo "show_apk3($ii,apk_status[$ii]);\n";
echo "</script>";


   echo "</div>";
   echo "</div>";



   echo "<div id=\"zipprogress" . $ii . "\" style=\"display:none\">";
   echo "<progress class=\"zipbutton zipmargin\" id=\"zipprog" . $ii . "\" max=\"11\" value=\"0\">70%</progress>";
   echo "</div>";
   echo "</a>";
   echo "</div>";
   echo "</div>";
   echo "<div id=\"scriptdialog$ii\" style=\"display:none\" class=\"scriptdialog\">";
   echo "<div id=\"scriptclose$ii\" class=\"scriptclose\">";
   echo "<button id=\"scriptclosebutton$ii\" onclick=\"hide_script($ii)\">[X]</button>";
   echo "</div>";
   echo "<div  class=\"scriptdialog_inner\">";
   echo "<pre id=\"scriptdialog_inner$ii\" style=\"font-size: 70%; line-height: 70%;\">";
   echo "</pre>";
   echo "</div>";
   echo "</div>";
   echo "<div id=\"copydialog$ii\" style=\"display:none\" class=\"copydialog\">";
   echo "<div id=\"copyclose$ii\" class=\"copyclose\">";
   echo "<button id=\"copyclosebutton$ii\" onclick=\"hide_copy($ii)\">[X]</button>";
   echo "</div>";
   echo "<div  class=\"copydialog_inner\">";
   echo "<pre id=\"copydialog_inner$ii\" style=\"font-size: 70%; line-height: 70%;\">";
   echo "</pre>";
   echo "</div>";
   echo "</div>";
}
echo "<script>";
echo "function js_parse(arr) {\n";
echo "  apk_status = JSON.parse(arr);\n";
echo "  apk_status.keys().forEach((key)=>{show_apk3(key,apk_status[key]);});";
echo "}\n";
echo "function js2_parse(arr) {\n";
echo "  addtext_status = JSON.parse(arr);\n";
echo "  addtext_status.keys().forEach((key)=>{show_copy3(key,addtext_status[key]);});";
echo "}\n";
echo "var apk_status=[];\n";
echo "fetch(\"https://meshpage.org/mesh_addtext.php?id=0&full=array\")\n";
echo "     .then(x=>x.text()).then(y=>js2_parse(y));\n";
echo "fetch(\"https://meshpage.org/mesh_apk.php?id=0&full=array\")\n";
echo "     .then(x=>x.text()).then(y=>js_parse(y));\n";
echo "var display_labels = " . json_encode($display_labels) . ";";
echo "</script>";

   echo "</div>";

   echo "<div style=\"font-family: 'calibri', sans-serif\" align=\"left\">";
   echo "<p>";
   echo "<div style=\"float:left; overflow: hidden; height: 1px; width:15px\"></div>";
   if ($startpos!="")
      echo "<a href=\"" . $site . "/meshpage.php\">000</a> | ";
   else echo "000 | ";
   if ($startpos!="1")
      echo "<a href=\"" . $site . "/meshpage.php?ps=1\">050</a> | ";
   else echo "050 | ";
   if ($startpos!="2")
      echo "<a href=\"" . $site . "/meshpage.php?ps=2\">100</a> | ";
   else echo "100 | ";
   if ($startpos!="3")
      echo "<a href=\"" . $site . "/meshpage.php?ps=3\">150</a> | ";
   else echo "150 | ";
   if ($startpos!="4")
      echo "<a href=\"" . $site . "/meshpage.php?ps=4\">200</a> | ";
   else echo "200 | ";
   if ($startpos!="5")
      echo "<a href=\"" . $site . "/meshpage.php?ps=5\">250</a> | ";
   else echo "250 | ";
   if ($startpos!="6")
      echo "<a href=\"" . $site . "/meshpage.php?ps=6\">300</a> | ";
   else echo "300 | ";
   if ($startpos!="7")
      echo "<a href=\"" . $site . "/meshpage.php?ps=7\">350</a> | ";
   else echo "350 | ";
   if ($startpos!="8")
      echo "<a href=\"" . $site . "/meshpage.php?ps=8\">400</a> | ";
   else echo "400 | ";
   if ($startpos!="9")
      echo "<a href=\"" . $site . "/meshpage.php?ps=9\">450</a> | ";
   else echo "450 | ";
   if ($startpos!="10")
      echo "<a href=\"" . $site . "/meshpage.php?ps=10\">500</a>";
   else echo "500";
   echo "</div>";
echo "<br>";
page_footer();
echo "<div style=\"height:40px\"></div>";
echo "</div>";
echo "<script>\n";
   echo "var g_background = 0;\n";
echo "function show_script2(ii,dt) {\n";
echo "   var d = document.getElementById(\"scriptdialog_inner\" + ii);\n";
echo "   dt = dt.replaceAll(\"<\",\"&lt;\");\n";
echo "   dt = dt.replaceAll(\">\",\"&gt;\");\n";
echo "   dt = dt.replaceAll(\"\\n\",\"<br>\");\n";
echo "   d.innerHTML = dt;\n";
echo "}\n";
echo "function show_script(v,ump) {\n";
echo "   var d = document.getElementById(\"scriptdialog\" + v);\n";
echo "   d.style = \"\";\n";
echo "   fetch(\"https://meshpage.org/mesh_pre.php?id=\" + ump)\n";
echo "     .then(x => x.text())\n";
echo "     .then(y => show_script2(v,y));\n";

echo "}";
echo "function hide_script(v) {\n";
echo "   var d = document.getElementById(\"scriptdialog\" + v);\n";
echo "   d.style = \"display:none;\"\n";
echo "}";
echo "function show_copy2(ii,dt) {\n";
echo "   var d = document.getElementById(\"copydialog_inner\" + ii);\n";
echo "   dt = dt.replaceAll(\"<p>\",\"\");\n";
echo "   dt = dt.replaceAll(\"</p>\",\"\");\n";
echo "   dt = dt.replaceAll(\"<\",\"&lt;\");\n";
echo "   dt = dt.replaceAll(\">\",\"&gt;\");\n";
echo "   dt = dt.replaceAll(\"\\n\",\"<br>\");\n";
echo "   dt = dt.trim();\n";
echo "   d.innerHTML = dt;\n";
echo "}\n";
echo "function show_copy(v,ump) {\n";
echo "   var d = document.getElementById(\"copydialog\" + v);\n";
echo "   d.style = \"\";\n";
      $arr = array("username" => "terop", "index" => $_GET["id"]);
      $res = addtext_date($arr);
      echo "var dt = \"$res;\";";
echo "   fetch(\"https://meshpage.org/mesh_addtext.php?id=\" + ump + \"&\" + dt )\n";
echo "     .then(x => x.text())\n";
echo "     .then(y => show_copy2(v,y));\n";

echo "}";
echo "function hide_copy(v) {\n";
echo "   var d = document.getElementById(\"copydialog\" + v);\n";
echo "   d.style = \"display:none;\"\n";
echo "}";
echo "</script>\n";

?>


<div id="main_display"> <!--v-show="state.mesh"-->
<div class="display" id="display2" style="display:none">
<div class="ems" id="canvas2" style="width:330px; height:247px"></div>
</div>
<div class="display_title_bar" id="display_title_bar">
</div>
<div class="display" id="display" > <!-- style="display:none" -->

<?php
require_once("user.php");
$mobile = js_mobile();
$connect = "no";

echo "<canvas class=\"ems\" id=\"gpucanvas\" style=\"width:330px; height: 247px; display:none;\" width=\"330\" height=\"247\" oncontextmeny=\"event.preventDefault()\" tabindex=-1></canvas>";

if ($mobile=="yes") {
echo "<canvas class=\"ems\" id=\"canvas\" style=\"width:330px; height:247px\" width=\"330\" height=\"247\" oncontextmenu=\"event.preventDefault()\" tabindex=-1></canvas>";
} else {
echo "<canvas class=\"ems\" id=\"canvas\" style=\"width:800px; height:600px\" width=\"800\" height=\"600\" oncontextmenu=\"event.preventDefault()\" tabindex=-1></canvas>";
}


?>
</div>
<p>
<p>
<div id="addtext"></div>
<?php
  page_footer2();
  ?>


</div>


<div v-if="state.about">
<p>
<p>
<p>
<h1>about and contact info</h1>
<?php
echo "Introduction: <a href=\"article.php\">article</a><br>";
echo "Create(private): <a href=\"mesh_login.php?return=create_mesh.php\">mesh</a><br>";
echo "Vault(private): <a href=\"mesh_login.php?return=mesh_hide.php\">vault</a><br>";
echo "Adverticement: <a href=\"https://www.youtube.com/watch?v=0UF0zIMI2xA&feature=youtu.be\">Video</a><br><p>";

echo "Resume cookie dialog: <a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">RESUME</a><br>";
echo "<script>function resume_cookies() { var c = document.getElementById(\"callout\"); c.style.display=\"block\"; }</script>"; 

echo "Technology demonstration: <a href=\"https://youtu.be/WZxCE-RsBDc\">Demo</a><br><p>";
echo "Video tutorial: <a href=\"https://meshpage.org/assets/builder_greeen_example.mp4\">tutorial</a><br><p>";

echo "Email address: terop@kotiposti.net<br>";
echo "Phone number: +358 50 5827126<br>";
echo "Y-tunnus: 3544765-4 (Tero Pulkkinen)<br>";
echo "Ko-fi address: <a href=\"https://ko-fi.com/terop57376\">terop57376</a><br>";
echo "<p>";
echo "Github: <a href=\"https://github.com/terop2/GameApi\">https://github.com/terop2/GameApi</a><br>";
$version_source = file_get_contents('https://meshpage.org/assets/VERSION_SOURCE.TXT');
echo "Source Code: <a href=\"" . $assetsite . "/GameApi-sourcecode-v$version_source.tar.gz\">GameApi-sourcecode-v$version_source.tar.gz</a>.";
echo "<br>Yours,<br><img loading=\"lazy\" src=\"" . $assetsite . "/avatar.png\" width=\"50\" height=\"50\" crossorigin></img>";
//echo "<a href=\"https://stackexchange.com/users/429879\"><img loading=\"lazy\" src=\"https://stackexchange.com/users/flair/429879.png\" width=\"208\" height=\"58\" alt=\"profile for tp1 on Stack Exchange, a network of free, community-driven Q&amp;A sites\" title=\"profile for tp1 on Stack Exchange, a network of free, community-driven Q&amp;A sites\" crossorigin></a>";
echo "<br><br>Development history of the project:<br>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2024.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2023.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2022.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2021.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2020.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2019.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2018.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2017.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2016.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2015.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2014.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2013.png\" crossorigin></img>";
echo "<img loading=\"lazy\" src=\"" . $assetsite . "/github_2012.png\" crossorigin></img>";

echo "<br><br><br><hr><br><br><br>";

echo "<hr>";
echo "Terms and conditions:<p>";
echo "<ul>";
echo "<li>You have explicit permission to use gameapi builder and <?php echo $sitename ?> for creating 3d model, the node graph, and deploy the end result to a web site of your choice. From builder tool you also have our permission to take screengrabs and videos using external screen grabbing software.";
echo "<li>You are not allowed to add copyright infringing content to any url slots inside builder tool. While builder can filter out some of that usage, the checks are not perfect and if that happens, we basically cannot do anything to protect our end users from the content owner's legal actions";
echo "<li>For children at ages 10-18 years old, gameapi builder provides safe learning environment where it is difficult to do large mistakes like publishing sexual material, copyright infringements or racist brainwashing.";
echo "<li>GameApi builder and <?php echo $sitename ?> web site does not create additional communication channels for human communication. This is to ensure that our end users do not suffer communication overload";
echo "<li><?php echo $sitename ?> is not a social media platform. Correct designation is \"Content Creation Tool Provider\" and  \"Web Publishing Enabler\" and also \"Content Publisher\"";
echo "<li>Standard disclaimers apply, no warranty or ability to return purchased material";
echo "<li>How we handle licensing is that we have chosen LGPLv3 to be our main license, but because some assets are GPL, the whole thing needs to be distributed under GPLv3. ";
echo "<li>External modules have their own licenses, including GPLv2, GPLv3, SIL Open Font license v1.10, CC0 creative commons license, zlib license, MIT license, Apache licese, public domain or no license required.";
echo "</ul>";
?>
<script>
 var g_counter = 0;
 function onload_button() {
    g_counter = g_counter + 1;
    if (g_counter<15)
       setTimeout(login, 50);
 }
 var g_user_id = "";
 var g_cnts = null;




function login() {

}
</script>

</div>
<div v-if="state.docs">
<p>
<p>
<p>
<h1>documents</h1>
<?php
require_once("user.php");
list_start("Available documents:");
list_item($assetsite . "/Linux_compile.txt", "Linux compilation");
list_item($site . "/feat.php", "Builder Feature list");
list_item($assetsite . "/Opengl_chain.txt", "Mesh structure");
list_item($assetsite . "/Math_concepts.txt", "Math concepts");
list_item($assetsite . "/Printer.txt", "3d printer instructions");
list_item($assetsite . "/Skills_to_learn.txt", "Skills to learn");
list_item($assetsite . "/Releasing_animations.txt", "Releasing animations to your web page");
list_item("https://meshpage.org/mesh_doc.php?menu=0&submenu=0&select=select&1831127721", "API reference manual");
list_end();
?>

</div>
<div v-if="state.faq">
<p>
<p>
<p>
<h1>frequently asked questions</h1>
<h2>What is <?php echo $sitename ?>?</h2>

<ul>
<li><?php echo $sitename ?> is groundbreaking way to bring web to next level. Next generation web will be 3d with <?php echo $sitename ?>'s technology.
</ul>

<h2>How does the site work?</h2>
<ul>
<li><a style="font-size:large;" href="<?php echo $site ?>/meshpage_4">DOWNLOAD</a>: You download the builder tool</a>
<li><a style="font-size:large;" href="<?php echo $assetsite ?>/builder_green4_example.webm">CREATE</a>: Create your powerful message with 3d technology
<li><a style="font-size:large;" href="<?php echo $assetsite ?>/Releasing_codegen.txt">CODEGEN</a>: You get piece of c++-like code representing animation
<li><a style="font-size:large;" href="<?php echo $assetsite ?>/Releasing_animations.txt">PUBLISH</a>: place 3d engine to your web server
<li><a style="font-size:large;" href="<?php echo $site ?>/engine2/index.html">ENJOY</a>: Then open the animation in your browser
</ul>
<h2>What are the advantages in your approach?</h2>
<ul>
<li>OpenGL based software-only solution, so no need to buy a camera
<li>File size is very small. The file size is like 30 lines of c++ code,
instead of megabytes of video files
<li>It's based on "designing the future", instead of "recording the past"
<li>Displaying models works via URL's: normal web technologies are used to transfer animations to other computers on the planet 
</ul>
<h2>What are the disadvantages?</h2>
<ul>
<li>Work amounts are higher than simply pressing "record" button in a camera.
<li>Designing 3d animations usually requires some planning
<li>The tool is slightly more complicated than we wanted
<li>Flexibility of the underlying software is always a problem
<li>End product sizes are pretty small, so you can't expect full length movie
    as the output of this process
<li>Different animation models are very limited -- basically just translate/rotate
</ul>

<h2>What to do if your hosting location has no php?</h2>
<ul>
<li>comment out or remove display.php 4 lines at the beginning
<li>you will lose threads too.
<li>every .zip file distributed by meshpage.org has one of those display.php files with this issue.
</ul>

<h2>What is the standard rendering pipeline?</h2>

<img src="https://meshpage.org/assets/std_render_pipeline.png" width="600" height="200"/>

<h2>And if you want something to move on screen?</h2>

<img src="https://meshpage.org/assets/std_rendering_with_move.png" width="600" height="200"/>

<h2>Is there a 3d model viewer that uses your engine?</h2>

Yes. It's <a href="<?php echo $site ?>/view.php" target="_blank">here</a>

Useful other sites which you can drag and drop content to the viewer are at least:
<ul>
<li><a href="https://sketchfab.com/3d-models?date=week&features=downloadable&sort_by=-likeCount" target="_blank">sketchfab</a>
<li><a href="https://www.thingiverse.com/" target="_blank">Thingiverse</a>
<li><a href="https://polyhaven.com/" target="_blank">PolyHaven</a>
</ul>

Useful other sites with gltf viewing capability in web:
<ul>
<li>modelviewer.dev
<li>sketchfab's 3d engine
<li>three.js
<li>meshpage.org's gltf_to_zip converter
<li>babylon.js
<li>playcanvas viewer
<li>loading the file in unity then using their webgl port
<li>loading the file in unreal engine using their webgl port
</ul>

<h2>If I just want to deploy simple gltf file?</h2>

Check our gltf-to-zip converter at <a href="https://meshpage.org/gltf_to_zip.php">Here</a>.

<h2>Now, if I got display.php, how do I embed it to my article</h2>

&lt;iframe scrolling="no" src="display.php" width="830" height="630"&gt;

<h2>I want smooth transition from jpg/png/webp to the 3d engine</h2>

There's now a callback for that purpose
<pre>
  Module.gameapi_cb = function()
   { var d = document.getElementById("img"); d.style="display:none"; }
</pre>

<h2>What is the minimal node graph that can be deployed to web?</h2>

<img loading="lazy" src="<?php echo $assetsite ?>/minimal_boxes.png" crossorigin/>
<p>
Important part for deployment is the properties of html_run, since it
contains url to your hosting space, which you need to change. This
ensures that any urls that you load inside the node graph can be
fetched from your hosting space. Unrestricted url access is not
allowed for copyright reasons.
<p>
Web server config(.htaccess) should be something like the following: (you should change the url)
<br>
<pre>
Header set Access-Control-Allow-Origin "<?php echo $site ?>" 
Header set Cross-Origin-Embedder-Policy "require-corp" 
Header set Cross-Origin-Resource-Policy "same-site" 
Header set Cross-Origin-Opener-Policy "same-origin" 
Header set Access-Control-Allow-Headers "range"
</pre>

Additionally, you should have in the page where iframe or embed is in, the following header definition:
<pre>
  &lt;?php
     header("Cross-Origin-Opener-Policy: same-origin");
  ?&gt;
</pre>
Apparently this needs to be in the page itself and not in the .htaccess to get SharedArrayBuffer and pthreads working.


<h2>How to setup http cache with the engine?</h2>

We recommend in .htaccess:
<pre>
Header set Cache-Control "max-age=604800, immutable"
</pre>
(this avoids small problem with browsers deciding different caching strategy for .wasm and .js files and serving mismatched files for the users, causing EM_ASM crashes)

<h2>What about compressing the huge engine files?</h2>

For nice performance speedup for network, you should enable brotli compression in the web server too:
<br>
.HTACCESS Changes:
<pre>
AddOutputFilterByType BROTLI_COMPRESS text/xml
AddOutputFilterByType BROTLI_COMPRESS text/css
AddOutputFilterByType BROTLI_COMPRESS application/xml
AddOutputFilterByType BROTLI_COMPRESS application/xhtml+xml
AddOutputFilterByType BROTLI_COMPRESS application/rss+xml
AddOutputFilterByType BROTLI_COMPRESS application/javascript
AddOutputFilterByType BROTLI_COMPRESS application/x-javascript
AddOutputFilterByType BROTLI_COMPRESS application/wasm
AddOutputFilterByType BROTLI_COMPRESS application/x-font-opentype
AddOutputFilterByType BROTLI_COMPRESS text/plain
&lt;files *.wasm&gt;
SetOutputFilter BROTLI_COMPRESS
&lt;/files&gt;
&lt;files *.ds.*&gt;
SetOutputFilter BROTLI_COMPRESS
&lt;/files&gt;
&lt;files *.ds&gt;
SetOutputFilter BROTLI_COMPRESS
&lt;/files&gt;
&lt;files *.data&gt;
SetOutputFilter BROTLI_COMPRESS
&lt;/files&gt;
AddType application/wasm wasm
AddType application/x-gameapi-binary ds
AddType text/plain mtl
AddType application/x-font-opentype otf
</pre>
Then /etc/apache2/sites-enabled/000-default.conf probably should have:
<pre>
       &lt;IfModule mod_brotli.c&gt;
        AddOutputFilterByType BROTLI_COMPRESS text/html text/plain text/xml text/css text/javascript application/javascipt application/wasm application/x-javascript
	&lt;/IfModule&gt;
	&lt;IfModule mod_headers.c&gt;
	RewriteCond "%{HTTPS:Accept-encoding}" "br"
	RewriteCond "%{REQUEST_FILENAME}\.br" "-s"
	RewriteRule "^(.*)\.(wasm)" "$1\.$2\.br" [QSA]
	RewriteRule "^(.*)\.(ds)\.(..)" "$1\.$2\.$3\.br" [QSA]

	RewriteRule "\.wasm\.br$" "-" [T=application/wasm,E=no-brotli:1]
	&lt;FilesMatch "(\.wasm\.br)$"&gt;
	  Header append Content-Encoding br
	  Header append Vary Accept-Encoding
	&lt;/FilesMatch&gt;
	&lt;FilesMatch "(\.ds\...\.br)$"&gt;
	  Header append Content-Encoding br
	  Header append Vary Accept-Encoding
	&lt;/FilesMatch&gt;
	&lt;/IfModule&gt;
</pre>

Then you need brotli packages:
<pre>
  sudo apt install brotli -y
  sudo a2enmod brotli
  sudo systemctl restart apache2
</ptr>


<h2>What technologies are you using to provide the features of the site?</h2>
<ul>
<li>C++
<li>php
<li>OpenGL
<li>SDL2
<li>glew
<li>emscripten
<li>libfreetype
<li>stb_image
<li>openvr
<li>holoplay
<li>tiny_gltf
<li>draco
<li>opencv videofile
<li>(normal browser environment)
</ul>
In addition to these external stuff, we have internal development in the following modules:
<ul>
<li>GameApi
<li>GameApi Builder/editor
<li>web_page
<li>mesh-page
<li>iot_main
<li>parser2
</ul>
<h2>What kind of features are available in the base system?</h2>
<ul>
<li>OpenGL instancing
<li>3d mesh data structure
<li>triangles/quads/lines/cones/spheres/cubes/torus
<li>colours/normals/texture coordinates
<li>materials like snow/brashmeta/web/distance fields
<li>shading
<li>animations/movement/matrices
<li>skeletal animations
<li>vertex animations
<li>recursive shapes/trees
<li>waveforms
<li>volumes
<li>2d and 3d rendering
<li>sprites / bitmaps
<li>circles/rectangles/triangles in 2d
<li>fonts/text rendering
<li>etc..
</ul>
<h2>Why do tool menus not do anything?</h2>
<ul>
<li>you can link boxes whenever the labels match
<li>display menu from right mouse button is disabled until linking has been completely done from the left side of the boxes -- this is because left side is inputs, and right side is outputs, and all inputs need to be available for box to work correctly
</ul>
<h2>What keybindings are there in the tool?</h2>
<ul>
<li>ctrl-s saves the data to mod.txt
<li>'a' rotates the model in most dialogs under "display"
<li>'d' rotates the model in most dialogs under "display"
<li>delete (in box title bar) deletes the box
<li>esc exits the application
<li>esc (in blk_window) exits the full screen mode
</ul>
<h2>Trackpads, touchpads and drawing tablets do not work with the technology?</h2>
This is known problem, but we don't have solution to the problem. The emscripten_set_wheel_callback() function sends events<br> that are not suitable for getting the trackpad to work properly. In ubuntu, we've found that changing settings in gnome and<br> disabling "two finger scroll" and enabling "edge scroll" can help it a little, but our current recommendation is to disable<br> the trackpad's completely from gnome settings.

The main problem is visible in zoom feature, when mouse wheel is being mapped to the zooming of the 3d models. The trackpad's<br> are not able to simulate mouse wheel accurately/consistently enough and we were unable to figure out why gnome's "two finger scroll"<br> sends always positive deltaY, when both negative and positive values should be available in the callback.

We haven't checked what needs to be done to get trackpads working in windows.

<h2>What tools you should try immediately?</h2>
<ul>
<li>m_snow, m_flat for shading
<li>anim_translate/anim_rotate for movement
<li>move_ml for movement
<li>seq_ml for moving from one scene to another
<li>cube/sphere/cone/torus for basic shapes
<li>p_render/m_bind for basic rendering
<li>p_render_inst/m_bind_inst for OpenGL instancing
<li>li_from_polygon/li_to_cone/random_quad for some amazing effects
<li>static_instancing/ms_static_inst for cpu side instancing
<li>blk_window and MainLoopApi features to finish your product
</ul>
<h2>I get browser crashes when loading your web site in windows machines</h2>
Here's steps to fix some webgl2 problems in windows:
<ul>
<li>about:flags -> angle graphics backend -> opengl
</ul>
It seems the default d3d11 webgl2 backend is somewhat broken, but happily
its possible to fix the problem by using the opengl backend.

<h2>I get error while loading the animations</h2>
<ul>
<li>With new pthreads support in the site, browser settings need to be changed
<li>Chrome users: chrome://flags should have webassembly pthreads enabled
<li>Firefox users: about:config shuold have shared.memory=true
</ul>
<h2>What browser should I use?</h2>
On my laptop I get the following benchmarks(this test: <a href="<?php echo $site ?>/mesh_display?id=GVILK@032">here</a>(
<ul>
<li>some users have reported 700fps, but with high-end video card
<li>Brave browser gives about 147fps
<li>firefox is 54fps
<li>Chrome is at 30fps
</ul>
<h2>What does blogs think about our site?</h2>
<ul>
<li>mediaisnothingtomebutistilllikeit <a href="https://mediaisnothingtomebutistilllikeit.wordpress.com/2017/12/06/meshpage-org/">blog</a>
</ul>
</div>
<div v-if="state.tool_download">
<p>
<link itemprop="applicationCategory" href="http://schema.org/ModellingTool">
<a itemprop="downloadUrl" href="<?php echo $assetsite ?>/GameApi-Builder-v<?php echo file_get_contents('https://meshpage.org/assets/VERSION_WIN.TXT'); ?>.msi">
<img loading="lazy" src="<?php echo $assetsite ?>/gameapi-builder-screenshot2.png" width="901" height="199" crossorigin></a>

<p>
<p>
<h1>builder tool download</h1>
<?php
require_once("user.php");
?>

PURCHASE LICENCES: <a href="pp/paypal.php" crossorigin referrerpolicy="no-referrer-when-downgrade">HERE</a> (LGPL/GPL)
<p><br>
MOST RECENT RELEASE: WIN: 
<?php
$version_win = file_get_contents('https://meshpage.org/assets/VERSION_WIN.TXT');
$version_win = substr($version_win, 0, -1);
echo "v." . $version_win . " ";
$start_time = filemtime("./assets/GameApi-Builder-v" . $version_win . ".msi");
$end_time = time();
$delta = floor(($end_time - $start_time)/60/60/24);
echo "(" . $delta . " days ago)";
?>
 <br>
MOST RECENT RELEASE: LINUX: 

<?php
$version = file_get_contents('https://meshpage.org/assets/VERSION.TXT');
$version = substr($version, 0, -1);
echo "v." . $version  . " ";
$start_time = filemtime("./gameapi-builder_1.0-" . $version . ".deb");
$end_time = time();
$delta = floor(($end_time - $start_time)/60/60/24);
echo "(" . $delta . " days ago)";
?>

<br>
MOST RECENT RELEASE: SOURCECODE: 

<?php
$version_source = file_get_contents('https://meshpage.org/assets/VERSION_SOURCE.TXT');
$version_source = substr($version_source, 0, -1);
echo "v." . $version_source . " ";
$start_time = filemtime("./GameApi-sourcecode-v" . $version_source . ".tar.gz");
$end_time = time();
$delta = floor(($end_time - $start_time)/60/60/24);
echo "(" . $delta . " days ago)";
?>
<br><br>
CONCEPT IMAGES GENERATED VIA ARTIFICIAL INTELLIGENCE(chatgpt4):<br>
<a href="<?php echo $site ?>/god_playing_with_builder.webp" target=_blank><img loading="lazy" src="<?php echo $site ?>/god_playing_with_builder.webp" width="200" height="200" crossorigin></img></a>
<a href="<?php echo $site ?>/world_transform.webp" target=_blank><img loading="lazy" src="<?php echo $site ?>/world_transform.webp" width="200" height="200" crossorigin></img></a>
<a href="<?php echo $site ?>/subtle_calmness.webp" target=_blank><img loading="lazy" src="<?php echo $site ?>/subtle_calmness.webp" width="200" height="200" crossorigin></img></a>
<a href="<?php echo $site ?>/city_building.webp" target=_blank><img loading="lazy" src="<?php echo $site ?>/city_building.webp" width="200" height="200" crossorigin></img></a>
<a href="<?php echo $site ?>/investor.webp" target=_blank><img loading="lazy" src="<?php echo $site ?>/investor.webp" width="200" height="200" crossorigin></img></a>
<br><br>
DOWNLOADING THE ACTUAL PRODUCT OFFERING:
<div style="padding: 0px; width: 1324px;">
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left; ">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi Builder</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">Modelling Tool, Gamedev</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Windows 11 64-bit</span>
<li><b>Download url:</b> <a href="<?php echo $assetsite ?>/GameApi-Builder-v<?php echo file_get_contents('https://meshpage.org/assets/VERSION_WIN.TXT'); ?>.msi">download msi</a>
<div itemprop="aggregateRating" itemscope itemtype="http://schema.org/AggregateRating">
  <ul>
  <li>Rating: <span itemprop="ratingValue">5.0</span>
  <li>RatingCount: <span itemprop="ratingCount">1</span>
  </ul>
</div>
<div itemprop="offers" itemscope itemtype="http://schema.org/Offer">
  <ul>
  <li>Price:<span itemprop="price">0.00</span>EUR
  <meta itemprop="priceCurrency" content="EUR" />
<?php
visit_counter_inc( "tool" );
?>
  </ul>
</div>
</ul>
</div>
<div style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left;">
<div style="margin: 30px;">
      (doubleclick msi file to install it)<br>
       start menu -> GameApi-Builder v<?php echo file_get_contents('https://meshpage.org/assets/VERSION_WIN.TXT'); ?>.0 -> GameApi_Builder v<?php echo file_get_contents('https://meshpage.org/assets/VERSION_WIN.TXT'); ?>.0
</div>
</div>

<div style="border-style: solid; width: 220px; height: 150px; background-color: white; float:left;">
<div style="margin: 10px;">
<img loading="lazy" src="Windows-Symbol.png" width="200" height="120" crossorigin/>
</div>
</div>

</div>
<p>
<div style="padding: 0px;  width: 1324px;">
<div></div>
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left; ">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi Builder</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">Modelling Tool, Gamedev</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Ubuntu 64-bit amd64</span>
<li><b>Download url:</b> <a href="<?php echo $assetsite ?>/gameapi-builder_1.0-<?php echo file_get_contents('https://meshpage.org/assets/VERSION.TXT'); ?>.deb">download deb</a>
<div itemprop="aggregateRating" itemscope itemtype="http://schema.org/AggregateRating" >
  <ul>
  <li>Rating: <span itemprop="ratingValue">5.0</span>
  <li>RatingCount: <span itemprop="ratingCount">1</span>
  </ul>
</div>
<div itemprop="offers" itemscope itemtype="http://schema.org/Offer">
  <ul>
  <li>Price:<span itemprop="price">0.00</span>EUR
  <meta itemprop="priceCurrency" content="EUR" />
  </ul>
</div>
</ul>
</div>
<div style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left;">
<div style="margin: 30px;">
      sudo dpkg -i gameapi-builder_1.0-<?php echo file_get_contents('https://meshpage.org/assets/VERSION.TXT'); ?>.deb<br>
      gameapi-builder
</div>
</div>


<div style="border-style: solid; width: 220px; height: 150px; background-color: white; float:left;">
<div style="margin: 15px 50px 15px 50px;">
<img loading="lazy" src="data:image/png;base64,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"
width="120" height="120" crossorigin/>
</div>
</div>

</div>

<p>
<div style="padding: 0px;  width: 1324px;">
<div></div>
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left; ">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi Builder</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">Modelling Tool, Gamedev</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Linux/Docker container</span>
<li><b>Graphics Card:</b> <span itemprop="graphicsCard">NVidia</span>
<li><b>Download url:</b> <a href="<?php echo $assetsite ?>/gameapi-builder-docker-container-<?php echo file_get_contents('https://meshpage.org/assets/VERSION.TXT'); ?>.tar.gz">download tar.gz</a>
<div itemprop="aggregateRating" itemscope itemtype="http://schema.org/AggregateRating" >
  <ul>
  <li>Rating: <span itemprop="ratingValue">5.0</span>
  <li>RatingCount: <span itemprop="ratingCount">1</span>
  </ul>
</div>
<div itemprop="offers" itemscope itemtype="http://schema.org/Offer">
  <ul>
  <li>Price:<span itemprop="price">0.00</span>EUR
  <meta itemprop="priceCurrency" content="EUR" />
  </ul>
</div>
</ul>
</div>
<div style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left;">
<div style="margin: 30px;">
      (install docker, nvidia-container-toolkit)<br>
      sudo docker build -t builder-test:latest .<br>
      ./run.sh
</div>
</div>


<div style="border-style: solid; width: 220px; height: 150px; background-color: white; float:left;">
<div style="margin: 15px 50px 15px 50px;">
<img loading="lazy" src="data:image/png;base64,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" width="120" height="120" crossorigin/>
</div>

</div>


<p>
<div style="padding: 0px;  width: 1324px;">
<div></div>
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left; ">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi CmdLine</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">GameDisplay tool</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Linux/Docker container</span>
<li><b>Graphics Card:</b> <span itemprop="graphicsCard">NVidia</span>
<li><b>Download url:</b> <a href="<?php echo $assetsite ?>/gameapi-builder-docker-cmdline-<?php echo file_get_contents('https://meshpage.org/assets/VERSION.TXT'); ?>.tar.gz">download tar.gz</a>
<div itemprop="aggregateRating" itemscope itemtype="http://schema.org/AggregateRating" >
  <ul>
  <li>Rating: <span itemprop="ratingValue">5.0</span>
  <li>RatingCount: <span itemprop="ratingCount">1</span>
  </ul>
</div>
<div itemprop="offers" itemscope itemtype="http://schema.org/Offer">
  <ul>
  <li>Price:<span itemprop="price">0.00</span>EUR
  <meta itemprop="priceCurrency" content="EUR" />
  </ul>
</div>
</ul>
</div>
<div style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left;">
<div style="margin: 30px;">
      (install docker, nvidia-container-toolkit)<br>
      sudo docker build -t builder-test:latest .<br>
      ./run.sh script.txt homepageurl<p>
      OR ON UBUNTU 25.04:<br>
      LD_LIBRARY_PATH=. ./gameapi_cmdline --file script.txt --homepage homepageurl<br>
</div>
</div>


<div style="border-style: solid; width: 220px; height: 150px; background-color: white; float:left;">
<div style="margin: 15px 50px 15px 50px;">
<img loading="lazy" src="data:image/png;base64,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" width="120" height="120" crossorigin/>
</div>
</div>



<p>
<div style="padding: 0px;  width: 1324px;">
<div></div>
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left; ">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi CmdLine</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">GameDisplay tool</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Windows</span>
<li><b>Download url:</b> <a href="<?php echo $assetsite ?>/GameApi_cmdline_<?php echo file_get_contents('https://meshpage.org/assets/VERSION.TXT'); ?>.zip">download zip</a>
<div itemprop="aggregateRating" itemscope itemtype="http://schema.org/AggregateRating" >
  <ul>
  <li>Rating: <span itemprop="ratingValue">5.0</span>
  <li>RatingCount: <span itemprop="ratingCount">1</span>
  </ul>
</div>
<div itemprop="offers" itemscope itemtype="http://schema.org/Offer">
  <ul>
  <li>Price:<span itemprop="price">0.00</span>EUR
  <meta itemprop="priceCurrency" content="EUR" />
  </ul>
</div>
</ul>
</div>
<div style="border-style: solid; width: 400px; height: 150px; background-color: white; float:left;">
<div style="margin: 30px;">
      gameapi_cmdline.exe --file script.txt --homepage homepageurl<p>
</div>
</div>


<div style="border-style: solid; width: 220px; height: 150px; background-color: white; float:left;">
<div style="margin: 10px;">
<img loading="lazy" src="Windows-Symbol.png" width="200" height="120" crossorigin/>
</div>
</div>




</div>
<!--
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; background-color: white;">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi Builder</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">Modelling Tool, Gamedev</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Web</span>
<li><b>Tool url:</b> <a href="<?php echo $site ?>/gameapi_builder.php">execute tool in web browser</a>
<div itemprop="aggregateRating" itemscope itemtype="http://schema.org/AggregateRating">
  <ul>
  <li>Rating: <span itemprop="ratingValue">5.0</span>
  <li>RatingCount: <span itemprop="ratingCount">1</span>
  </ul>
</div>
<div itemprop="offers" itemscope itemtype="http://schema.org/Offer">
  <ul>
  <li>Price:<span itemprop="price">0.00</span>EUR
  <meta itemprop="priceCurrency" content="EUR" />
  </ul>
</div>
</ul>
</div>
-->
<br><p>

<div>

<a href="<?php echo $assetsite ?>/builder_screenshot.webp" target=_blank><img loading="lazy" src="<?php echo $assetsite ?>/builder_screenshot.webp" width="300" height="200" crossorigin></img></a>
<a href="<?php echo $assetsite ?>/builder_screenshot2.webp" target=_blank><img loading="lazy" src="<?php echo $assetsite ?>/builder_screenshot2.webp" width="300" height="200" crossorigin></img></a>
<a href="<?php echo $assetsite ?>/builder_screenshot3.webp" target=_blank><img loading="lazy" src="<?php echo $assetsite ?>/builder_screenshot3.webp" width="300" height="200" crossorigin></img></a>
</div>
</div>


</div>
</div>

</div> <!-- app.. vue ends here -->
</body>
<style>
.buttons {
   position: absolute;
   right: 10px;
   top: 50px;
   display: flex;
   width: 300px;
}
.butinner {
   padding: 20px 20px 20px 20px;
   width: 300px;
   float: left;
   border-radius: 14px;
   border-style: outset;
   border-width: 3px;
   margin: 5px;
   background-color: #fff;
}
.butinner:hover {
   background-color: #ccf;
}
.ziphoriz {
  display: inline-flex;
}
.scriptdialog {
   position: fixed;
   top: 50px;
   width: 800px;
   height: 600px;
   border-style: solid;
   border: 2px solid;
   z-index: 300;
   background-color: white;
   overflow: scroll;
   scrollbar-width: thin;
}
@media screen and (max-width: 480px) {
.scriptdialog {
   position: fixed;
   top: 50px;
   width: 320px;
   height: 600px;
   border-style: solid;
   border: 2px solid;
   z-index: 300;
   background-color: white;
   overflow: scroll;
   scrollbar-width: thin;
}
}
.scriptclose {
   position: relative;
   left: 750px;
}
@media screen and (max-width: 480px) {
.scriptclose {
   position: relative;
  left: 285px;
}
}
.scriptdialog_inner {
   margin: 30px;
}
.copydialog {
   position: fixed;
   top: 50px;
   width: 800px;
   height: 200px;
   border-style: solid;
   border: 2px solid;
   z-index: 300;
   background-color: white;
   overflow: scroll;
   scrollbar-width: thin;
}
@media screen and (max-width: 480px) {
.copydialog {
   position: fixed;
   top: 50px;
   width: 320px;
   height: 600px;
   border-style: solid;
   border: 2px solid;
   z-index: 300;
   background-color: white;
   overflow: scroll;
   scrollbar-width: thin;
}
}
.copyclose {
   position: relative;
   left: 750px;
}
@media screen and (max-width: 480px) {
.copyclose {
   position: relative;
  left: 285px;
}
}
.copydialog_inner {
   margin: 30px;
}
.zipbutton {
   position: relative;
   left: 54px;
   top: 10px;
}
.zipmargin {
   z-index: 100;
}
.logo {
  position: absolute;
  right: -4px;
  top: 37px;
}
.tab {
  overflow: hidden;
  border: 1px solid #ccc;
  background-color: #f1f1f1;
  }
.tab button {
  background-color: inherit;
  float: left;
  border: none;
  outline: none;
  cursor: pointer;
  padding: 14px 16px;
  transition: 0.3s;
  }
.tab button:hover {
  background-color: #ddd;
}
.tab button.active {
  background-color: #ccc;
}
.display_title_bar {
   font-size: 200%;
   text-align: center;
   height: 40px;
}
.tabcontent {
  padding: 6px 12px;
  border: 1px solid #ccc;
  border-top: none;
  }
.notdefault {
  display: none;
  }
.dropdown-window {
  position: absolute;
  top:36px;
  right: 0px;
  background-color: #000;
  z-index: 1;
  border-width: thin;
  border-style: solid;
  border-color: #fff;
}
.dropdown-content a {
  color: white;
  padding: 12px 16px;
  text-decoration: none;
  display: block;
}
.dropdown-content
{
font-size: 20px;
width: 240px;
text-color: #fff;
}
.dropdown-content a:hover { background-color: #d40; }
.noselect {
 -webkit-touch-callout: none; /* iOS Safari */
    -webkit-user-select: none; /* Safari */
     -khtml-user-select: none; /* Konqueror HTML */
       -moz-user-select: none; /* Firefox */
        -ms-user-select: none; /* Internet Explorer/Edge */
            user-select: none; /* Non-prefixed version, currently
                                  supported by Chrome and Opera */
}
.ems {
  border-width:0px;
  border: 5px solid black;
  border-radius: 10px;
  background-color: #000;
  margin: 0;
  padding: 0;
  width: 820px;
  height: 620px;
  }
#canvas {
<?php
if ($sitename=="meshpage.org") {
echo "   background-image: url('load_spinner2.gif');";
} else
{
echo "   background-image: url('dino4.webp');";
}
?>
   background-size: 100% 100%;
   background-repeat: no-repeat;
   background-position: center;
}
</style>
<script>

function hide_display(b)
{
  var elem = document.getElementById("main_display");
  if (elem) {
  if (b) elem.style="";
  else elem.style="display:none";
}
}

function hide_main(b)
{
  var elem = document.getElementById("main_div");
  if (elem) {
  if (b) elem.style="";
  else elem.style="display:none";
  if (b) {
     //populate_imgs();
  }
  }
}

function event_target(event)
{
}

var store = {
  state: {
    empty: true,
    main: false,
    create_mesh: false,
    tool_download: false,
    faq: false,
    docs: false,
    about: false,
    login: false,
    vault: false,
    block: false,
    stats: false,
    logout: false,

    mesh: false,
    functions: false,
    source_code: false,
    html_embed: false,
    dropdown: false,

    profile: false,
    create_new: false,
    my_animations: false
    },
    clear_state() {
       this.state.empty = false;
       this.state.main = false;
       this.state.create_mesh = false;
       this.state.tool_download = false;
       this.state.faq = false;
       this.state.docs = false;
       this.state.about = false;
       this.state.login = false;
       this.state.vault = false;
       this.state.block = false;
       this.state.stats = false;
       this.state.logout = false;
       this.state.mesh = false;
       this.state.functions = false;
       this.state.source_code = false;
       this.state.html_embed = false;
       this.state.profile = false;
       this.state.create_new = false;
       this.state.my_animations = false;
      hide_display(this.state.mesh);
      hide_main(this.state.main);
      },
    toggle_dropdown()
    {
	if (this.state.dropdown) {
	this.state.dropdown=false;
	     var dd = document.getElementById("dropdown");
	     dd.style = "display:none";

	}
	else {
	     this.state.dropdown=true;
	     var dd = document.getElementById("dropdown");
	     dd.style = "display:inline;";
	     var hm = document.getElementById("html");
	     function clickhandler(state,hm) {
	      return function (event)
	     	      {
			var elem = document.getElementById("login_label");
			if (!elem.contains(event.target))
			   {
		  		state.dropdown=false;
				var dd = document.getElementById("dropdown");
	     			dd.style = "display:none";

				hm.removeEventListener("click", clickhandler, true);
     	 	           }
		      
	     	      }
	     }
	     hm.addEventListener("click", clickhandler(this.state,hm), true);
	     }
    },
    choose(a) {
      this.clear_state();
      if (a=='main') this.state.main = true;
      if (a=='create_mesh') this.state.create_mesh = true;
      if (a=='tool_download') this.state.tool_download = true;
      if (a=='faq') this.state.faq = true;
      if (a=='docs') this.state.docs = true;
      if (a=='about') this.state.about = true;
      if (a=='login') this.state.login = true;
      if (a=='vault') this.state.vault = true;
      if (a=='block') this.state.block = true;
      if (a=='stats') this.state.stats = true;
      if (a=='logout') this.state.logout = true;
      if (a=='mesh') this.state.mesh = true;
      if (a=='functions') this.state.functions = true;
      if (a=='source_code') this.state.source_code = true;
      if (a=='html_embed') this.state.html_embed = true;
      if (a=='profile') this.state.profile = true;
      if (a=='create_new') this.state.create_new = true;
      if (a=='my_animations') this.state.my_animations = true;
      hide_display(this.state.mesh);
      hide_main(this.state.main);
      },
};

function create_id2( name, index )
{
  var str = name.concat("@".concat(index));
  var begin = "abcdefghijklmnopqrstuvwxyz1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ@";
  var end = "ZYXWVUTSRQPONMLKJIHGFEDCBA0987654321zyxwvutsrqponmlkjihgfedcba@";
  var i = 0;
  for(i=0;i<str.length;i++) {
    var j = 0;
    for(j=0;j<begin.length;j++) {
	if (str[i] == begin[j]) { str[i] = end[j]; }
	}
    }
    return str;
}


function zip_progress3(id)
{
  return function() {
   		     var el = document.getElementById("zipprogress" + id.toString());
		     el.style="display:none";   
		     var el2 = document.getElementById("zipprog" + id.toString());
   		     el2.value = "0";
  }
}

var g_zip_count = 0;
function zip_progress2(id)
{
  return function() {
   var el = document.getElementById("zipprog" + id.toString());
   g_zip_count++;
   if (el.value!="11" && g_zip_count<50) {
 const myHeaders3 = new Headers();
 const myBRequest = new Request("find_zip_status.php", {
   method: 'GET',
   headers: myHeaders3,
   mode: 'same-origin',
   cache: 'default'
});
   fetch(myBRequest).then((r) => {
      return r.text();
}).then((t) => {
   var el = document.getElementById("zipprog" + id.toString());
   el.value = t;
});
   window.setTimeout(zip_progress2(id),500);
} else {
   window.setTimeout(zip_progress3(id),2000);
		     
}
}
}

function zip_progress(id)
{
   return function() {
   		     var el = document.getElementById("zipprogress" + id.toString());
   		     el.style="";
		     g_zip_count=0;
		     window.setTimeout(zip_progress2(id),500);		     
		     }
}


var app = new Vue({
   el: '#app',
   mounted: function() {
          var vm = this;
     choose_breadlist(0,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
     //start_emscripten(vm);
     window.onpopstate = function(e) {
       stop_music();
       var st = e.state;
       //console.log(st);
       //console.log(e);
       if (st==null) {
       	  choose_breadlist(0,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);

       	  store.choose('main');
	  if (typeof fix_keyboard === "function") fix_keyboard(true);
	  return;
	  }
       var pgnum = e.state.page;

       var url = window.location.href;
       var n = url.search("_");
       var n2 = url.search("page=");
       var cut = 1;
       if (n2>n) { n=n2; cut=5; }	 
       var i;
       var res = "";
       for(i=0;i<url.length-n-cut;i++) {
         var ch = url[i+n+cut];
	 if (ch>='0' && ch<='9') res+=ch;
	 else break;
       }

       n = url.search("id=");
       var res2 = "";
       for(i=0;i<url.length-n-3;i++) {
         var ch = url[i+n+3];
	 if (ch>='0' && ch<='9') res2+=ch;
	 else break;
       }
       //n=url.search("label=");
       //var res3 = url.substr(n+6);

       pgnum = parseInt(res);
       var id = parseInt(res2);
       var label = create_id2( "terop", res2); //res3;
       //console.log(res);
       //console.log(pgnum);
       if (pgnum==3||pgnum==1) {
          //console.log("MAIN");
	  choose_breadlist(0,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);

	  store.choose('main');
	  //start_emscripten(vm);
	  //if (pgnum==1)
	  //   window.history.go(-2);
	  //else
	  //   window.history.go(-1);
	  if (typeof fix_keyboard === "function") fix_keyboard(true);
	  }
       if (pgnum==2) {
          //console.log("MESH");
       	  window.mesh_display(vm,id,label); 
	  if (typeof fix_keyboard === "function") fix_keyboard(false);
	  }
       }
     //window.history.replaceState({page:1},"title 1", "?p=1");
     window.mesh_display = function(vue,id,label) {
               if (!g_emscripten_alive) return;
          var vm = vue;
	  choose_breadlist(1,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
	  choose_display(id,label,vm,true);
       }

<?php
//var_dump($_GET);
$id = $_GET["id"];
//$label = $_GET["label"];
$label = create_id2( "terop", $id);
$page = $_GET["p"];
if ($page=="") $page = $_GET["page"];
//var_dump($id);
//var_dump($label);
if ($page!="") {
   if ($id!="" && $label!="") {
   echo "g_last_id=\"$id\";";
   echo "g_last_label=\"$label\";";
   }
   if ($page==1) {
      echo "store.choose(\"main\");";
      echo "choose_bread(\"main\",vm.main_breadcrumb);";
      echo "if (typeof fix_keyboard === \"function\") fix_keyboard(true);";
   }
   if ($page==2) {
      echo "setTimeout(delayed_choose_display(vm,g_last_id,g_last_label),1);";
      echo "store.choose(\"mesh\");";

      echo "if (typeof fix_keyboard === \"function\") fix_keyboard(false);";
      } else
      {
      echo "choose_breadlist(0,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);";
      }
   if ($page==3) {
      echo "store.choose(\"main\");";
      echo "choose_bread(\"main\",vm.main_breadcrumb);";
      echo "if (typeof fix_keyboard === \"function\") fix_keyboard(true);";
   }
   if ($page==4) {
      echo "store.choose(\"tool_download\");";
      echo "choose_bread(\"tool_download\",vm.main_breadcrumb);";
      echo "if (typeof fix_keyboard === \"function\") fix_keyboard(true);";
   }
   if ($page==5) {
      echo "store.choose(\"faq\");";
      echo "choose_bread(\"faq\",vm.main_breadcrumb);";
      echo "if (typeof fix_keyboard === \"function\") fix_keyboard(true);";
   }
   if ($page==6) {
      echo "store.choose(\"docs\");";
      echo "choose_bread(\"docs\",vm.main_breadcrumb);";
      echo "if (typeof fix_keyboard === \"function\") fix_keyboard(true);";
   }
   if ($page==7) {
      echo "store.choose(\"about\");";
      echo "choose_bread(\"about\",vm.main_breadcrumb);";
      echo "if (typeof fix_keyboard === \"function\") fix_keyboard(true);";
   }

} else {
  echo "store.choose(\"main\");";
  echo "choose_bread(\"main\",vm.main_breadcrumb);";
      echo "if (typeof fix_keyboard === \"function\") fix_keyboard(true);";
}

?>



},
   beforeDestroy() {
   },
   computed: {
      isIndicator2() { return this.indicator[1]; },
      isIndicator3() { return this.indicator[2]!=2 && this.indicator[2]!=0; },
      isIndicator3_2() { return this.indicator[2]==2; },
      isIndicatorNone() { return this.indicator[2]==0; }
   },
   methods: {

       mesh_display(id,label,display_label) {
       	  var d = document.getElementById("display_title_bar");
	  d.innerHTML = display_label;
	  if (g_focus4) {
	     return;
	  }
	  if (g_focus3) {
	     return;
	  }
          if (g_focus2) {
	     return;
	  }
       	  if (g_focus) {
       	  var frm = document.getElementById("form" + id.toString());
	  frm.submit();
	  window.setTimeout(zip_progress(id),10);
          return;
          }
          if (cookie_status==0) return;
		hide_profile(false);
          var vm = this;
          if (!g_emscripten_alive) { start_timer(id,label,vm); return; }
	  choose_breadlist(1,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
	  choose_display(id,label,vm,false);

	  //console.log(id);
       },
       login_click(e) {
	  store.toggle_dropdown();
       },
       login_menu_click(e,str) {
       		store.choose(str);
		hide_profile(str=='profile');
       },
       bread_click(e) {
		hide_profile(false);
       		      var vm = this;
		      var txt = e.target.textContent;
       		      //console.log(txt);
		      choose_breadcrumb(txt,vm.main_breadcrumb,store,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
    		      },

   },
   data: {
     state: store.state,
     main_breadcrumb_second: [{num:0,name: "mesh_all", choose:"main", title:"<?php echo "$sitename"; ?>", link:false},
     {num:1,name:"mesh_display",choose:"mesh",title:"mesh display", link:false},
     ],
     main_breadcrumb_first: [{num:0,name: "mesh_all", choose:"main", title:"<?php echo "$sitename"; ?>", link:true},
     		       //{num:1,name: "create_mesh", choose:"create_mesh",title:"create mesh",link:false},
		       {num:2,name: "mesh_tool", choose:"tool_download",title:"tool download",link:false},
                       {num:3,name: "mesh_faq", choose:"faq",title:"faq",link:false},
		       {num:4,name: "mesh_tutorial", choose:"docs",title:"docs",link:false},
		       {num:5,name: "mesh_about", choose:"about", title:"about",link:false},
		       //{num:6,name: "mesh_hide.php", choose:"vault",title:"vault",link:false},
		       //{num:8,name: "mesh_block", choose:"block",title:"block",link:false},
		       //{num:9,name: "mesh_stats", choose:"stats", title:"stats",link:false},
		       //{num:10,name: "mesh_logout", choose:"logout",title:"logout",link:false},
		       //{num:7,name: "mesh_login_page", choose:"login",title:"login",link:false}
		       ],
		       main_breadcrumb : [],
		       indicator: [true,true,0],
   },
   directives: {
   }
   });



var g_tm_cb;

function timer_timeout(id,label,vm)
{
return function() {
    //console.log("TIMER TICK");
       if (!g_emscripten_alive) {
    //console.log("NOT ALIVE");
       if (g_tm_cb) window.clearTimeout(g_tm_cb);
       g_tm_cb = window.setTimeout(timer_timeout(id,label,vm), 10);
       return;
       } else {
    //console.log("ALIVE");
	  choose_display(id,label,vm,false);

      }
}

}

function start_timer(id, label, vm)
{
	  choose_breadlist(1,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
	  store.choose("mesh");

//console.log("START TIMER");
    if (g_tm_cb) window.clearTimeout(g_tm_cb);
      g_tm_cb = window.setTimeout(timer_timeout(id,label,vm), 10);
}

function choose_bread(txt,breadcrumb)
{
   var sz = breadcrumb.length;
   var i;
   for(i=0;i<sz;i++) {
      var bread = breadcrumb[i];
      if (txt=="main" && bread.title=="<?php echo $sitename ?>") bread.link=true; else
      if (bread.title==txt) {
         bread.link = true;
      } else {
        bread.link = false;
	}
	}
}

function choose_breadcrumb(txt,breadcrumb,store,first,second)
{


   var sz = breadcrumb.length;
   var i;
   for(i=0;i<sz;i++) {
      var bread = breadcrumb[i];
      if (bread.title==txt) {
         bread.link = true;
      	 var choose = bread.choose;
	 //console.log(choose);
	 store.choose(choose);
	 if (breadcrumb.length == first.length) {
	     window.history.replaceState({page: 2},"title 2", "/meshpage_" + (i+3).toString());
	     } else {
	     if (i==0) {
	     window.history.back();
//window.history.replaceState({page: 2},"title 2", "meshpage_" + (i+1).toString());
	     }
	     }

      } else {
        bread.link = false;
      }
   }

      if (txt=='<?php echo $sitename ?>') {
          choose_breadlist(0,breadcrumb,first,second);
	  if (typeof fix_keyboard === "function") fix_keyboard(true);
	  }
      if (txt!='mesh') {
           var elem = document.getElementById("display");
	   elem.style.display = "none";
	   var elem2 = document.getElementById("display2");
	   elem2.style.display = "block";
	  if (typeof fix_keyboard === "function") fix_keyboard(true);

      } else { 	  if (typeof fix_keyboard === "function") fix_keyboard(false); }


}
function choose_breadlist(id,breadcrumb,first,second)
{
  while(breadcrumb.length) breadcrumb.pop();
  var target;
  if (id==0) { target = first; }
  if (id==1) { target = second; }

  var s = target.length;
  var i;
  for(i=0;i<s;i++) breadcrumb.push(target[i]);

}
function delayed_choose_display(vm, id, label)
{
  return function() {
   if (!g_emscripten_alive) {
      setTimeout(delayed_choose_display(vm,id,label), 100);
      return;
  }
  //console.log(id);
  //console.log(label);

   choose_breadlist(1,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
   choose_display(id,label,vm,true);

}
}

var g_last_id = "";
var g_last_label = "";

var g_txt = {};
var g_txt_id = 0;
var g_txt_cb = null;
function choose_display_timeout(vm)
{
   //console.log("CHOOSE DISPLAY TIMEOUT");
   var vm2 = vm;
   return function() {
	if (g_txt[g_txt_id]!==undefined) {
	show_emscripten(g_txt[g_txt_id],false,vm2.indicator,true);
	g_txt_cb = null;
	g_txt_id = 0;
	}
   else
      {
      g_txt_cb = window.setTimeout(choose_display_timeout(vm2), 100);
      }
      }
}


var m_id = 0;

function choose_display(id,label, vm,is_popstate)
{
   var d = document.getElementById("display_title_bar");
   d.innerHTML = display_labels[id];


  m_id = id;
  if (!is_popstate) {
  g_last_id = id;
  g_last_label = label;
  	       // ?p=2&id="
  window.history.pushState({page: 2},"title 2", "/" + id.toString() ); // + "&label=" + label.toString()
  }
  store.choose("mesh");
  choose_breadcrumb("mesh display",vm.main_breadcrumb,store,vm.main_breadcrumb_first,vm.main_breadcrumb_second);

  var url = "<?php echo $site ?>/mesh_pre.php?id=" + label;
<?php
      $arr = array("username" => "terop", "index" => $_GET["id"]);
      $res = addtext_date($arr);
      echo "var dt = \"$res;\";";
?>
  var url2 = "<?php echo $site ?>/mesh_addtext.php?id=" + label + "&" + dt;
  var url3 = "<?php echo $site ?>/mesh_background.php?id=" + label;
  //console.log(g_txt[id]);
  //console.log(url3);
  if (g_txt[id]===undefined) {

const myHeaders3 = new Headers();
const myBRequest = new Request(url3, {
   method: 'GET',
   headers: myHeaders3,
   mode: 'same-origin',
   cache: 'default'
});
//console.log("GOING TO BACKGROUND 0");
fetch(myBRequest).then((r) => {
    //console.log("GOING TO BACKGROUND");
   return r.text();
}).then((t) => {
   if (t=="") t="0";
   //console.log("BACKGROUND:" + t);
   g_background = parseInt(t,10);
});   

const myHeaders2 = new Headers();
const myARequest = new Request(url2, {
      method: 'GET',
      headers: myHeaders2,
      mode: 'same-origin',
      cache: 'default'
      });
      fetch(myARequest).then((r) => {
      	return r.text();
	}).then((t) => {
  	   var pos = document.getElementById("addtext");
	   pos.innerHTML = t;
	   });

const myHeaders = new Headers();
const myFRequest = new Request(url, {
  method: 'GET',
  headers: myHeaders,
  mode: 'same-origin',
  cache: 'default'
  });
  if (g_txt_cb) window.clearTimeout(g_txt_cb);
  g_txt_cb = window.setTimeout(choose_display_timeout(vm), 30);

  fetch(myFRequest).then((r) => {
    return r.text();
  }).then((t) => {
    //console.log("INDICATOR:");
    //console.log(vm.indicator);
    g_txt_id = id;
    g_txt[id] = t;
    //
  });
  } else {
    g_txt_id = id;
    if (g_txt_cb) window.clearTimeout(g_txt_cb);
    g_txt_cb = window.setTimeout(choose_display_timeout(vm), 1);
  }

}
var g_emscripten_running = false;
var g_emscripten_start_count = 0;


var g_keyboard_focus_timeout = null;

function set_keyboard_focus()
{
  //var iframe = document.getElementById('canvas');
  //iframe.contentWindow.focus();
  g_keyboard_focus_timeout = null;
}

function remove_keyboard_focus_from_iframe()
{
   //var h = document.getElementById("html");
   //h.contentWindow.focus();
}

function set_keyboard_focus_to_iframe()
{
  if (g_keyboard_focus_timeout)
     window.clearTimeout(g_keyboard_focus_timeout);
  g_keyboard_focus_timeout = window.setTimeout(set_keyboard_focus,100);
}
function start_emscripten_really(vm)
{
//	 console.log(ASM_CONSTS);
     var str = "";
     str+="P I1=ev.polygon_api.p_empty();\n";
     str+="ML I2=ev.polygon_api.render_vertex_array_ml2(ev,I1);\n";
     str+="RUN I3=ev.blocker_api.game_window2(ev,I2,false,false,0.0,100000.0);\n";

    //console.log("INDICATOR2:");
    //console.log(vm.indicator);
     show_emscripten(str,true,vm.indicator,true);
}
function start_emscripten(vm)
{
  setTimeout(function() { start_emscripten_really(vm) },1000);
}

var g_check_em_timeout = null;
var g_ready_bit=0; // THIS WILL BE CHANGED BY C++ SIDE
var g_emscripten_alive = false;
function check_em(indicator) {
  return function() {
  	       //Module['onRuntimeInitialized'] = function() { return function() { } };
               if (g_check_em_timeout != null)
  	          clearTimeout(g_check_em_timeout);
	       g_check_em_timeout = null;
  	       //console.log("CHECK_EM");
	       //console.log(Module);
	       g_emscripten_alive=true;

//indicator[0]=false; 
	       var old = indicator[1];
	       var old2 = indicator[2];
	       indicator.pop();
	       indicator.pop();
	       indicator.pop();
	       //console.log("INDICATOR:");
	       //console.log(indicator);
	       indicator.push(false);
	       indicator.push(old);
	       indicator.push(old2);
	       var d = document.getElementById("engstatus");
	       d.innerHTML = "SUCCESS <a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
	       	setTimeout(function() { enable_spinner(false); },100);


		//Module.FS.mkdir('/my_heap');
		//Module.FS.mount(MEMFS, { files: Module['wasmMemory'].buffer }, '/my_heap');

	       return;
	       }
}
function check_emscripten_running(indicator)
{
  var canv = document.getElementById("canvas");
  //var canv2 = document.querySelector("#canvas");
  //var ctx = canv2.getContext("webgpu");
  //console.log("WEBGPUCONTEXT:");
  //console.log(ctx);
  if (Module) {
         //console.log(Module);
     	 //Module.ready().then(_ => check_em(indicator));
	 //Module['calledRun'] = check_em(indicator);
	 //Module['onRuntimeInitialized'] = check_em(indicator);
	 g_check_em_timeout = setTimeout(function() {
	                    //console.log("waiting for emscripten startup..");
	 		    if (g_ready_bit==1) {
	 		       /* console.log("onRuntimeInitialized didn't trigger in time");*/  check_em(indicator)(); } },1000);
	 //check_em(indicator);
   	 //console.log("RUNNING");
	 } else {
   	 //console.log("TIMEOUT");
	 setTimeout(function() { check_emscripten_running(indicator) },1000);
	 }
   //console.log("RUNNING");
   //console.log(indicator);
   //var ifrm = document.getElementById("ems");
   //console.log(ifrm);
   //if (ifrm!=null) {
   //   var cwin = ifrm.contentWindow;
   //   console.log(cwin);
   //   cwin.indicator = indicator;
   //   if (cwin != null) {
   //         Module = cwin.Module;
 //  	    console.log(Module);
  // 	    if (Module!=null) {
//	     return;
//	     }
 //     }
 //  }
 //  
}

function check_if_emscripten_running(indicator)
{
   setTimeout(function() { check_emscripten_running(indicator) },1000);
}


function resize_size() {
  return [830,630];
}

var gameapi_homepageurl = "<?php echo $assetsite ?>";

var g_display_timeout = null;
function display_cb()
{

              var elem = document.getElementById("display");
	      elem.style.display = "block";
              var elem2 = document.getElementById("display2");
	      elem2.style.display = "none";
	      
	      g_display_timeout = null;
}


var g_submit_timeout = null;
function submit_cb()
{
   //var frm = document.getElementById('submitform');
   //frm.submit();

   g_submit_timeout = null;
}

function stop_music()
{
   if (g_emscripten_running) {
     Module.ccall('stop_music_playing', null, [], [], { async:true });
     //Module._stop_music_playing();
   }
}
function enable_spinner(a)
{
  var el = document.getElementById("canvas2");
  var el2 = document.getElementById("canvas");
  if (a) {
  //  console.log("SPINNER ENABLED");
<?php
if ($sitename=="meshpage.org") {
echo "el.style.backgroundImage = \"url('load_spinner2.gif')\";";
} else
{
echo "el.style.backgroundImage = \"url('dino4.webp')\";";
}
?>
  el.style.backgroundRepeat = "no-repeat";
  el.style.backgroundPosition = "center";

    el.style.backgroundSize = "100% 100%";
} else {
   // console.log("SPINNER DISABLED");
    //el.style= "border-width:0px; border: 5px solid black; border-radius: 10px; background-color: #000; margin: 0; padding: 0;";
    el.style.backgroundImage = "none";
    el.style.backgroundColor = "#000000";
    el.style.backgroundSize = "auto auto";
    //el2.style= "border-width:0px; border: 5px solid black; border-radius: 10px; background-color: #000; margin: 0; padding: 0;";
    el2.style.backgroundImage = "none";
    el2.style.backgroundColor = "#000000";
    el2.style.backgroundSize = "auto auto";
    
  }
}

function show_emscripten(str,hide,indicator,is_async)
{
	if (!g_emscripten_running)
	  enable_spinner(true);

	if (typeof fix_keyboard === "function") fix_keyboard(hide);

	g_emscripten_start_count++;
	if (g_emscripten_running) {
	   str+="\n";
	   try {
	       if (is_async) {

	       	  Module.ccall('set_string', null, ['number', 'string'],[0,str],{async:false});
		  Module.ccall('set_background_mode', null, ['number'], [g_background], {async:true});
		  Module.ccall('set_integer', null, ['number','number'],[26,m_id], {async:true});


		  Module.ccall('set_string', null, ['number', 'string'],[5,g_user_id]);
		  } else {

		  Module.ccall('set_string', null, ['number', 'string'],[0,str]);
		  Module.ccall('set_background_mode', null, ['number'], [g_background]);
		  Module.ccall('set_integer', null, ['number','number'],[26,m_id]);

		  Module.ccall('set_string', null, ['number', 'string'],[5,g_user_id]);
		  }		  
	   } catch(e) {
	     console.log(e);
	   }
	   if (!hide) {
	      if (g_display_timeout)
	        window.clearTimeout(g_display_timeout);
	      g_display_timeout = window.setTimeout(display_cb,280); // 750
	      set_keyboard_focus_to_iframe();
	      } else {
	      remove_keyboard_focus_from_iframe();
	      }
	   return;
	} else {
	  g_emscripten_start_count = 0;
	
	var elem = document.getElementById("display");
	if (!hide) {
	
	   elem.style.display = "block";
	   var elem2 = document.getElementById("display2");
	   elem2.style.display = "none";
	   

	}
   	var html = "";
	var sz = resize_size();
        str = str.replace(/\n/g,"@");
        str = str.replace(/\r/g,"");
        str = str.replace(/</g,"");
        str = str.replace(/\t/g,"");

	if (g_submit_timeout) window.clearTimeout(g_submit_timeout);
	g_submit_timeout = window.setTimeout(submit_cb, 100);

	check_if_emscripten_running(indicator);
	g_emscripten_running = true;
	if (!hide) {
	   set_keyboard_focus_to_iframe();
        } else {
	  remove_keyboard_focus_from_iframe();
	}
	return;
	}

}


</script>
<script type="text/javascript">
  var canv = document.getElementById("canvas");

  var Module = { };
Module.canvas = canv;
  Module.locateFile = function(path,prefix) {
        if (path.endsWith('.js')) {
            if (!window.wasmJSContent) {
                // Synchronously fetch the JS content once
                var xhr = new XMLHttpRequest();
                xhr.open('GET', prefix + path, false);  // false makes it synchronous
                xhr.send(null);
                window.wasmJSContent = xhr.responseText;
            }
            // Create a Blob URL that can be reused
            if (!window.wasmJSUrl) {
                var blob = new Blob([window.wasmJSContent], {type: 'application/javascript'});
                window.wasmJSUrl = URL.createObjectURL(blob);
            }
            return window.wasmJSUrl;
        }
return prefix+path+"?<?php echo filemtime("engine_highmem.js") ?>"; }
Module.arguments = [

<?php
require_once("user.php");
$mobile = js_mobile();
if ($mobile=="yes") {
  echo "\"--size\", \"330\", \"247\",";
}
else {
echo "\"--size\", \"800\", \"600\",";
}
$ua = $_SERVER["HTTP_USER_AGENT"];
?>

      "--code", "P I1=ev.polygon_api.p_empty();\nML I2=ev.polygon_api.render_vertex_array_ml2(ev,I1);\nRUN I3=ev.blocker_api.game_window2(ev,I2,false,false,0.0,100000.0);\n", "--homepage",gameapi_homepageurl,"--platform","<?php echo $ua ?>"];
Module.print = (function() {
      	     return function(text) {
	     	       console.log(text);
		    } })();
</script>
<?php
/*
require_once("user.php");
$nothreads = js_no_threads();
$mobile = js_mobile();
$highmem = js_highmem();
if ($mobile == "yes") {
   echo "<script>import Module from './engine_lowmem_nothreads.js';</script>";
  //echo "<script src='engine_lowmem_nothreads.js?" . filemtime("engine_lowmem_nothreads.js") . "'></script>";
} else
if ($nothreads == "yes") {
   if ($highmem == "yes") {
  //echo "<script src='engine_nothreads_highmem.js?" . filemtime("engine_nothreads_highmem.js") . "'></script>";
   echo "<script async>import Module from './engine_nothreads_highmem.js';</script>";

   } else {
 // echo "<script src='engine_nothreads.js?" . filemtime("engine_nothreads.js") . "'></script>";
   echo "<script async>import Module from './engine_nothreads.js';</script>";

}
} else {
   if ($highmem == "yes") {
 // echo "<script src='engine_highmem.js?" . filemtime("engine_highmem.js") . "' crossorigin='anonymous'></script>";
   echo "<script async>import Module from './engine_highmem.js';</script>";

   } else {
  //echo "<script src='engine.js?" . filemtime("engine.js") . "' crossorigin='anonymous'></script>";
   echo "<script async>import Module from './engine.js';</script>";
  }
}
*/
?>
<script>

window.onresize = resize_event;
window.setTimeout(function() { resize_event(null); },10);


function resize_event(event) {
  var wd = window.innerWidth;
  var hd = window.innerHeight;
  var wd2 = wd/800.0;
  var hd2 = hd/600.0;
  if (wd2>hd2) wd2=hd2; else
  if (hd2>wd2) hd2=wd2;

  var hd3 = hd2;

  wd2*=800.0;
  hd2*=600.0;
  wd=wd2;
  hd=hd2;
<?php
require_once("user.php");
$mobile = js_mobile();
if ($mobile=="yes") {
  echo "wd-=60;";
  echo "hd-=60;";
} else {
  echo "wd-=240;";
  echo "hd-=240;";
  }
?>
  var elem = document.getElementById("display");
  var elem2 = document.getElementById("display2");
  if (typeof(elem)!='undefined' && elem != null) { } else { return; }
  elem.style.width = (wd).toString() + "px";
  elem.style.height = (hd).toString() + "px";
  elem2.style.width = (wd).toString() + "px";
  elem2.style.height = (hd).toString() + "px";

  var title = document.getElementById("display_title_bar");
  if (!title) return;
  title.style.width = (wd).toString() + "px";

  var iframe = document.getElementById("canvas");
  if (!iframe) return;
  iframe.style.width = (wd).toString() + "px";
  iframe.style.height = (hd).toString() + "px";

  var iframe2 = document.getElementById("canvas2");
  if (!iframe2) return;
  iframe2.style.width = (wd).toString() + "px";
  iframe2.style.height = (hd).toString() + "px";

  var iframe3 = document.getElementById("gpucanvas");
  if (!iframe3) return;
  iframe3.style.width = (wd).toString() + "px";
  iframe3.style.height = (hd).toString() + "px";


  //var prog = document.getElementById("prgress");
  //prog.style.width = (wd).toString() + "px";
  //var hdd = 30*hd3;
//prog.style.height = hdd.toString() + "px";

  if (g_emscripten_alive && g_emscripten_running && Module) {
	   try {
Module.ccall('set_resize_event', null, ['number', 'number'], [wd,hd], {async:true});
	//Module._set_resize_event(wd,hd);
	   } catch(e) {
	     console.log(e);
	   }

  }

}

</script>
<script>
function connect() {
   //var connection = new RTCPeerConnection({
   //    iceServers: [
   //      { 'url': 'stun:stun.1.46.32.252.120:424242' }]});
   //const signalingChannel = new SignalingChannel(connection);
   //console.log("signaling done");

   //const config {'iceServers':[{'urls': 'stun:stun.1.46.32.252.120:42424'}]}
   //const peerConnection = new RTCPeerConnection(configuration);
   //console.log("peer done");
}
</script>
<script>
function join_click()
{
   var inp = document.getElementById("connectid");
   var id_txt = inp.value;
   console.log("Join:" + id_txt);
}
function new_game_click()
{
   var inp = document.getElementById("gamename");
   console.log(inp);
   var id = 666555444;
   var sp = document.getElementById("displayconnectid");
   console.log(sp);
   sp.innerHTML = id.toString();
}
</script>
<script>
function e1(event)
{
  event.stopImmediatePropagation();
}

function fix_keyboard(hide)
{
  if (hide) {
  //console.log("FIX KEYBOARD TRUE");
  window.addEventListener('keydown', e1, true);
  window.addEventListener('keyup', e1, true);
  } else {
  //console.log("FIX KEYBOARD FALSE");
  window.removeEventListener('keydown', e1, true);
  window.removeEventListener('keyup', e1, true);
  }
}
function hide_profile(b)
{
   var elem = document.getElementById("profile");
   if (b) elem.style="";
   else elem.style="display:none";
}


//window.addEventListener('message', event=>{
//   console.log("MESSAGE EVENT");
//   if (event.origin.startsWith('https://meshpage.org')||event.origin.startsWith('http://meshpage.org')) {
//      g_cnts = event.data;
//      console.log("ADDEVENTLISTENER");
//      console.log(event.data);
//   } else {
//      return;
//   }
//});

</script>

<!-- TODO: add nft ownership test before starting timer -->
<script>
//setTimeout(function() { var d = document.getElementById("monepopup"); d.style.display='block'; }, 500000);
var url = "TODO"; //"https://api.opensea.io/asset/0x2953399124f0cbb46d2cbacd8a89cf0599974963/63044780828468072905070356195984022355064566916094515504044574281931983783584/validate";
function add_script() {

<?php
require_once("user.php");
$nothreads = js_no_threads();
$mobile = js_mobile();
$highmem = js_highmem();
if ($mobile == "yes") {
  echo "var filename = 'engine_lowmem_nothreads.js?" . filemtime("engine_lowmem_nothreads.js") . "';";
} else
if ($nothreads == "yes") {
   if ($highmem == "yes") {
  echo "var filename = 'engine_nothreads_highmem.js?" . filemtime("engine_nothreads_highmem.js") . "';";

   } else {
  echo "var filename = 'engine_lowmem_nothreads.js?" . filemtime("engine_lowmem_nothreads.js") . "';";
   }
} else {
   if ($highmem == "yes") {
  echo "var filename = 'engine_highmem.js?" . filemtime("engine_highmem.js") . "';";

   } else {
  echo "var filename = 'engine_lowmem_nothreads.js?" . filemtime("engine_lowmem_nothreads.js") . "';";
  }
}
?>

  const s = document.createElement('script');
  s.setAttribute('async', '');
  s.setAttribute('src', filename);
  s.setAttribute('crossorigin', 'anonymous');
  document.body.appendChild(s);
}
function fetch_wallet() {
const myHeaders2 = new Headers();
//myHeaders2.append('Accept', 'application/json');
//myHeaders2.append('X-API-KEY', ''); // TODO, GET API KEY ONCE HAVE SOME ETH 
//myHeaders2.append('Access-Control-Expose-Headers', 'X-API-KEY');
//myHeaders2.append('Origin', 'https://meshpage.org');
// TODO, sharedArrayBuffer + cors is not working.
/*
const myReq = new Request(url, {
      method: 'GET',
      headers: myHeaders2,
      mode: 'no-cors',
      cache: 'no-cache'
});
fetch(myReq,{"credentials":"include"}).then((r)=> {
	//console.log(r);
	return r.text();
}).then((t) => {
   //console.log("T:");
   //console.log(t);
   var d = document.getElementById("status");
   if (t["success"]==false) {
     d.innerHTML = "FAILED (api key missing) <a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
     var old2 = app.indicator[2];
     app.indicator.pop();
     app.indicator.pop();
     app.indicator.push(true);
     app.indicator.push(old2);
   } else {
     d.innerHTML = "SUCCESS (implementation unfinished, waiting for apikey) <a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
     var old2 = app.indicator[2];
     app.indicator.pop();
     app.indicator.pop();
     app.indicator.push(false);
     app.indicator.push(old2);
     //app.indicator[1]=true;
   }
}).catch(function(error) {
   var d = document.getElementById("status");
     d.innerHTML = "FAILED (" + error + ") <a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";    
     var old2 = app.indicator[2];
     app.indicator.pop();
     app.indicator.pop();
     app.indicator.push(true);
     app.indicator.push(old2);
     //app.indicator[1]=false;
});
*/
   var d = document.getElementById("status");
     d.innerHTML = "SUCCESS (implementation unfinished, waiting for apikey) <a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
     var old2 = app.indicator[2];
     app.indicator.pop();
     app.indicator.pop();
     app.indicator.push(false);
     app.indicator.push(old2);
}
function set_cookie_status(num)
{
   var promise = new Promise((resolve,reject) => {
   const head = new Headers();
   const req = new Request("./cookies.php?op=1&value=" + num.toString(10), {
      method: 'GET',
      headers: head,
      cache: 'no-cache'
      });
   fetch(req).then((r) => { return r.text(); }).then((t) => { resolve("ok"); });
   });
   return promise;
}
function check_country_cookies_async()
{
return new Promise((resolve) => { resolve("true"); });
/*
   const head3 = new Headers();
   const req3 = new Request("https://api.country.is/");
return new Promise((resolve) => {
   fetch(req3).then((r)=> {
      return r.text();
      }).then((ip) => {
      const json = JSON.parse(ip);
      const country = json["country"];
      console.log(country);
      if (country=="AT"||
      	 country=="BE"||
	country=="BG"||
	country=="HR"||
	country=="CY" ||
	country=="CZ"||
	country=="FI"||
	country=="DK"||
	country=="EE"||
	country=="FR"||
	country=="DE"||
	country=="GR"||
	country=="HU"||
	country=="IE"||
	country=="IT"||
	country=="LV"||
	country=="LT"||
	country=="LU"||
	country=="MT"||
	country=="NL"||
	country=="PL"||
	country=="PT"||
	country=="RO"||
	country=="SK"||
	country=="SI"||
	country=="ES"||
	country=="SE"||
	country=="EU")
      {
	resolve("false");
      } else {
	resolve("true");
      }
      });
});
*/
}
//async function check_country_cookies()
//{
//   const result = await check_country_cookies_async();
//   return result;
//}
function get_cookie_status()
{

   const head = new Headers();
   const req = new Request("./cookies.php?op=0", {
      method: 'GET',
      headers: head,
      cache: 'no-cache'
      });
   fetch(req).then((r)=> {
       return r.text();
       }).then((t) => {
          //console.log("FOUND COOKIE");
	  //console.log(t);
	  var t2 = t.trim();
	    var pg = "<?php echo $page ?>";
	  if (t2=="-1") { // no info available
	    if (!(pg==""||pg=="0"||pg=="3")) {
	       accept_cookies(false);
	    } else {
	    check_country_cookies_async().then(function(a){
	    accept_cookies(false);
		}, function(a) {
		var co = document.getElementById("callout");
	        co.style.display="block";
		});
		}
	  } else
	  if (t2=="0") { // reject cookies
	     reject_cookies(false);
	  } else
	  if (t2=="1") { // accept necessary cookies
	     accept_necessary_cookies(false);
	  } else
	  if (t2=="2") { // accept cookies
	     accept_cookies(false);
	  } else
	  {
	    console.log("COOKIE SYSTEM BROKEN");
	    console.log(t2);
	  }
       });
	

}
var cookie_status = -1;
function remove_cookies(num)
{
  const promise = new Promise((resolve,reject) => {
  if (num==0 || num==1) {
   const head = new Headers();
   const req = new Request("./cookies.php?op=2", {
      method: 'GET',
      headers: head,
      cache: 'no-cache'
      });
   fetch(req).then((r) => { return r.text(); }).then((t) => { console.log(t); resolve("OK") });    
  }
  });
  return promise;
}
function reject_cookies(save)
{
   cookie_status = 0;
   var es = document.getElementById("engstatus");
   es.innerHTML = "COOKIE REJECTED..<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
   var es = document.getElementById("status");
   es.innerHTML = "COOKIE REJECTED..<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
   var d = document.getElementById("loginstatus");
   d.innerHTML = "COOKIE REJECTED..<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
   var co = document.getElementById("callout");
   co.style.display='none';
   remove_cookies(0).then((ok) => {
   set_cookie_status(0).then((ok2) => {
      if (save) {  location.reload(); }
   });
   });
}
function accept_cookies(save)
{
   cookie_status = 2;
   var es = document.getElementById("engstatus");
   es.innerHTML = "LOADING..<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
   var es = document.getElementById("status");
   es.innerHTML = "LOADING..<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
   var d = document.getElementById("loginstatus");
   d.innerHTML = "CHECKING FOR CERTIFICATE..<!--a href=\"https://enroll.euderco.net/static/index.html\">?</a-->|<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";

   onload_button(); // uses cookie for oauth2.
   var co = document.getElementById("callout");
   co.style.display='none';
   add_script();
   start_emscripten(app);
   //setTimeout(function() { fetch_wallet(); }, 50);
   if (save) { set_cookie_status(2).then((ok)=>{ location.reload();}); }
}
function accept_necessary_cookies(save)
{
   cookie_status = 1;

   var es = document.getElementById("engstatus");
   es.innerHTML = "LOADING..<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
   var es = document.getElementById("status");
   es.innerHTML = "COOKIE REJECTED..<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
   var d = document.getElementById("loginstatus");
   d.innerHTML = "COOKIE REJECTED..<a href=\"JavaScript:void(0);\" onClick=\"resume_cookies()\">?</a>";
   var co = document.getElementById("callout");
   co.style.display='none';
   add_script();
   start_emscripten(app);
   //fetch_wallet();
   remove_cookies(1).then((ok) => {
   if (save) { set_cookie_status(1).then((ok2)=>{ location.reload(); }); }
   });
}
get_cookie_status();
</script>

<div id="callout" class="callout" style="display:none">
  <div class="callout-header">EU Cookie Popup</div>
  <!--span class="closebtn" onclick="this.parentElement.style.display='none';">&times;</span-->
  <div class="callout-container">
    <p>We regret to inform you that we need cookies, local filesystem and index database and certificate store on your computer to keep the site up and running. It's necessary for the correct operation of the site, but hopefully you agree with this usage. We use cookies for these purposes:</p>
    <p>
    <ol>
    <li>storage (emscripten filesystem, preload cache)
    <li>functionality (3d engine)
    <li>identification (login system)
    <li>monetization (purchase system, wallets)
    </ol>
    <div class="cnts">
    <div class="button" onclick="reject_cookies(true)"><b>Reject Cookies</b><br>(none)</div>
    <div class="button" onclick="accept_necessary_cookies(true)"><b>Accept Necessary Cookies</b><br>(1)(2)</div>
    <div class="button" onclick="accept_cookies(true)"><b>Accept Cookies</b><br>(1)(2)(3)(4)</div>
     </div>
  </div>
</div>

<div id="monepopup" class="callout" style="display:none">
  <div class="callout-header">Monetisation popup</div>
  <span class="closebtn" onclick="this.parentElement.style.display='none';">&times;</span>
  <div class="callout-container">
    <p>We have designed a way to purchase our technology via nft using ethereum, so now we ask you to purchase it <a href="https://terop.itch.io/gameapi-builder">here with money</a>, <a href="https://opensea.io/assets/matic/0x2953399124f0cbb46d2cbacd8a89cf0599974963/63044780828468072905070356195984022355064566916094515504044574281931983783584">or here with eth</a></p>
    <p>For eth, you need to have ethereum wallet available, for example <a href="https://www.coinbase.com/wallet">coinbase wallet</a>.
  </div>
</div>


<style>
.cnts {
  height: 100px;
  margin-left: 15px;
}
.button {
   background: #fff;
   width: 15%;
   height: 70px;
   margin-left: 5px;
   margin-right: 5px;
   margin-top: 10px;
   margin-bottom: 10px;
   padding: 15px;
   float: left;
}
.button:hover {
   background: #f88;
   width: 15%;
   height: 70px;
   float: left;
}

.callout {
  position: fixed;
  bottom: 35px;
  right: 20px;
  margin-left: 20px;
  max-width: 500px;
}
.callout-header {
  padding: 25px 15px;
  background: #555;
  font-size: 30px;
  color: white;
}
.callout-container {
  padding: 15px;
  background-color: #ccc;
  color: black;
}
.closebtn {
  position: absolute;
  top: 5px;
  right: 15px;
  color: white;
  font-size: 30px;
  cursor: pointer;
}
.closebtn:hover {
  color: lightgrey;
}
</style>