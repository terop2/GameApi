<?php
header("Cross-Origin-Opener-Policy: same-origin");
header("Cross-Origin-Embedder-Policy: require-corp");
?>
<html id="html">
<head>
<title>meshpage.org -- teleport animations to the world</title>
<?php
$page = $_GET["p"];
if ($page=="") $page = $_GET["page"];
if ($page!="2") {
 echo '<meta name="description" content="Meshpage allows converting your ideas to 3d for your web page"/>';
 }
 ?>
<meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, shrink-to-fit=no"/>
<meta name="verifyownership" content="e77986b70c2f57469a1bbea0b80ca188"/>
<meta http-equiv="origin-trial" content="AvkuION9OjDj+c5KxD0L/wgqyzkqE1vqOyceYiQe5LanN5395ZBJ/xfUuZcw7Mu7JkWiEskFjKGghchsKVVBKw4AAABYeyJvcmlnaW4iOiJodHRwczovL21lc2hwYWdlLm9yZzo0NDMiLCJmZWF0dXJlIjoiV2ViQXNzZW1ibHlUaHJlYWRzIiwiZXhwaXJ5IjoxNTYzOTI2Mzk5fQ==">
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
if ($mobile == "yes") {
  echo "<link rel=\"preload\" href=\"web_page_lowmem_nothreads.js?" . filemtime("web_page_lowmem_nothreads.js") . "\" as=\"script\"/>";
} else
if ($nothreads == "yes") {
   echo "<link rel=\"preload\" href=\"web_page_nothreads.js?" . filemtime("web_page_nothreads.js") . "\" as=\"script\"/>";

  echo "<link rel=\"preload\" href=\"web_page_nothreads.worker.js?" . filemtime("web_page_nothreads_worker.js") . "\" as=\"script\" crossorigin/>";
} else {
   echo "<link rel=\"preload\" href=\"web_page.js?" . filemtime("web_page.js") . "\" as=\"script\" crossorigin/>";
  //echo "<link rel=\"preload\" href=\"web_page.worker.js?" . filemtime("web_page.worker.js") . "\" as=\"script\" crossorigin/>";
  //echo "<link rel=\"preload\" href=\"web_page.worker.js\" as=\"script\" crossorigin/>";
}
echo "<link rel=\"preload\" href=\"mesh_css.css?" . filemtime("mesh_css.css") . "\" as=\"style\"  onload=\"this.rel = 'stylesheet'\"/>";

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
        "@id": "https://meshpage.org/meshpage?p=3",
	"name": "meshpage.org"
	}
  },
  {
     "@type": "ListItem",
     "position": 2,
     "item": {
        "@id": "https://meshpage.org/meshpage?p=4",
	"name": "tool download"
     }
  },
  {
     "@type": "ListItem",
     "position": 3,
     "item": {
        "@id": "https://meshpage.org/meshpage?p=5",
	"name": "faq"
     }
  },
  {
     "@type": "ListItem",
     "position": 4,
     "item": {
        "@id": "https://meshpage.org/meshpage?p=6",
	"name": "documents"
     }
  },
  {
     "@type": "ListItem",
     "position": 5,
     "item": {
        "@id": "https://meshpage.org/meshpage?p=7",
	"name": "about"
     }
  }
  ]
}
</script>
<body id="body">
<script src="https://meshpage.org/vue.js"></script>
<div id="app">
<div id="navbar" class="navi">
<div class="ncenter noselect">
<!--
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
-->

<template v-for="bread in main_breadcrumb">
<div style="font-family: 'calibri', sans-serif" class="link level1">
<meta itemprop="position" v-bind:content="bread.num"/>
<template v-if="bread.link">
<b>
<a class="navi" v-on:click="bread_click($event)"><span>{{ bread.title }}</span></a>
</b>
</template>
<template v-if="!bread.link">
<a class="navi" v-on:click="bread_click($event)"><span>{{ bread.title }}</span></a>
</template>
</div>
</template>
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
<div v-if="state.main">
<?php
require_once("user.php");
$user="terop";
$num = read_num( $user );
page_title("meshpage.org", "teleporting your 3d animations to all over the world");
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

   $url = "/" . $i; //"meshpage.php?p=2&id=" . $i; // . "&label=" . $id;
   echo "<div class=\"flex-item\" itemscope itemtype=\"http://schema.org/CreativeWork\">";
   echo "<div class=\"highlight\">";
   echo "<a class=\"label\" href=\"$url\" v-on:click.prevent=\"mesh_display(" . $i . ",'" . $id . "')\" itemprop=\"url\">";
   echo "<div class=\"border\">";
   echo "<div class=\"image\">";
   // BACKGROUND CHANGE
   echo "<img width=\"200\" height=\"150\" draggable=\"false\" src=\"" . $filename . "\" itemprop=\"thumbnailUrl\" crossorigin/>";


   //echo "<canvas id=\"cnv" . $i . "\" width=\"200\" height=\"150\"></canvas>";
  // echo "<script>\n";
  // echo "var g_background = 0;\n";
  // echo "function change_black_pixels_to_background(canvas, url, bg)\n";
  // echo "{\n";
  // echo "var filename = \"https://meshpage.org/bg\" + bg.toString() + \".png\";\n";
  // echo "var img = new Image;\n";
  // echo "img.src = filename;\n";
  // echo "var canvas = document.getElementById(canvas);\n";
   ////echo "img.onload = function() {\n";

  // echo "var filename2 = \"https://meshpage.org/bg_white.png\";\n";
  // echo "var white = new Image;\n";
  // echo "white.src=filename2;\n";

  // echo "var ctx = canvas.getContext(\"2d\");\n";
  // echo "var image = new Image();\n";
  // echo "image.src = url;\n";
  // echo "image.onload = function() {\n";
  // echo "  ctx.globalCompositeOperation = \"source-over\";\n";
  // echo "  ctx.drawImage(image, 0,0, canvas.width, canvas.height);\n";
  // echo "  var imgdata = ctx.getImageData(0,0,canvas.width,canvas.height);\n";
  // echo "  ctx.globalCompositeOperation = \"source-over\";\n";
  // echo "  ctx.drawImage(img, 0,0, canvas.width, canvas.height);\n";
  // echo "  var imgdata2 = ctx.getImageData(0,0,canvas.width,canvas.height);\n";
  // echo "  var dt = imgdata.data;\n";
  // echo "  var dt2 = imgdata2.data;\n";
  // echo " for(var y=0;y<canvas.height;y++) {\n";
  // echo " for(var x=0;x<canvas.width;x++) {\n";
  // echo "   if (dt[(x+y*canvas.width)*4+0]<8 && dt[(x+y*canvas.width)*4+1]<8 && dt[(x+y*canvas.width)*4+2]<8) {\n";
  // echo "    dt[(x+y*canvas.width)*4+0] = dt2[(x+y*canvas.width)*4+0];\n";
  // echo "    dt[(x+y*canvas.width)*4+1] = dt2[(x+y*canvas.width)*4+1];\n";
  // echo "    dt[(x+y*canvas.width)*4+2] = dt2[(x+y*canvas.width)*4+2];\n";
  // echo "   } } }\n";
  // echo " ctx.putImageData(imgdata,0,0);\n";
  // echo " delete imgdata; delete imgdata2;\n";
////echo "  ctx.globalCompositeOperation = \"difference\";\n";
 //  //echo "  ctx.drawImage(white, 0,0, canvas.width, canvas.height);\n";
 //  //echo "  ctx.globalCompositeOperation = \"difference\";\n";
 //  //echo "  ctx.putImageData(imgdata,0,0);\n";
 //  echo "}\n";
 //  echo "}\n";
 //  //echo "}\n";

//echo "var url3 = \"https://meshpage.org/mesh_background.php?id=" . $i . "\";\n";
//echo "{const myHeaders3 = new Headers();\n";
//echo "const myBRequest = new Request(url3, {\n";
//echo "   method: 'GET',\n";
//echo "   headers: myHeaders3,\n";
//echo "   mode: 'same-origin',\n";
//echo "   cache: 'default'\n";
//echo "});\n";
//echo "fetch(myBRequest).then((r) => {\n";
//echo "   return r.text();\n";
//echo "}).then((t) => {\n";
//echo "   if (t==\"\") t=\"1\";\n";
////echo"   console.log(\"BACKGROUND:\" + t);\n";
//echo "change_black_pixels_to_background(\"cnv" . $i . "\", \"" . $filename . "\", parseInt(t,10));\n";
//echo "});}\n";   
//echo "</script>\n";

   // END OF BACKGROUND CHANGE
   echo "</div>";
   echo "</div>";
   echo "<div style=\"font-family: 'calibri', sans-serif\" class=\"label\" align=\"center\">$label</div>";
   echo "</a>";
   echo "</div>";
   echo "</div>";
}
   echo "</div>";
   page_footer();
?>
</div>

<div v-show="state.mesh">

<div class="display" id="display" style="display:none">
<?php
require_once("user.php");
$mobile = js_mobile();
$connect = "no";
if ($mobile=="yes") {
echo "<canvas class=\"ems\" id=\"canvas\" style=\"width:330px; height:247px\" width=\"330\" height=\"247\" oncontextmenu=\"event.preventDefault()\" tabindex=-1></canvas>";
} else {
echo "<canvas class=\"ems\" id=\"canvas\" style=\"width:800px; height:600px\" width=\"800\" height=\"600\" oncontextmenu=\"event.preventDefault()\" tabindex=-1></canvas>";
}

if ($connect=="yes") {
  echo "Connect ID: <div contentEditable=\"true\">enter id</div>";
  echo "<button type=\"button\" onclick=\"join_click()\">Join</button>";
  echo "<br>";
  echo "Game Name: <div contentEditable=\"true\"></div><button onclick=\"new_game_click()\" type=\"button\">New Game</button>";
  echo "<div id=\"displayconnectid\"></div>";
} else {
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
echo "Adverticement: <a href=\"https://www.youtube.com/watch?v=0UF0zIMI2xA&feature=youtu.be\">Video</a><br><p>";

echo "Technology demonstration: <a href=\"https://youtu.be/WZxCE-RsBDc\">Demo</a><br><p>";

echo "Email address: terop@kotiposti.net<br>";
echo "Phone number: +358 50 5827126<br>";
echo "<p>";
echo "Github: <a href=\"https://github.com/terop2/GameApi\">https://github.com/terop2/GameApi</a>";
echo "<br>";

?>
</div>
<div v-if="state.docs">
<p>
<p>
<p>
<h1>documents</h1>
<?php
require_once("user.php");
list_start("Available documents:");
list_item("http://tpgames.org/Linux_compile.txt", "Linux compilation");
list_item("http://tpgames.org/Feature_list.txt", "Builder Feature list");
list_item("http://tpgames.org/Opengl_chain.txt", "Mesh structure");
list_item("http://tpgames.org/Math_concepts.txt", "Math concepts");
list_item("http://tpgames.org/Printer.txt", "3d printer instructions");
list_item("http://tpgames.org/Skills_to_learn.txt", "Skills to learn");
list_item("http://tpgames.org/Releasing_animations.txt", "Releasing animations to your web page");
list_end();
?>

</div>
<div v-if="state.faq">
<p>
<p>
<p>
<h1>frequently asked questions</h1>
<h2>What is meshpage.org?</h2>

<ul>
<li>Meshpage is a mechanism to create 3d animations and sending them to your friends.
</ul>

<h2>How does the site work?</h2>
<ul>
<li><a style="font-size:large;" href="https://meshpage.org/meshpage?p=4">DOWNLOAD</a>: You download the builder tool</a>
<li><a style="font-size:large;" href="https://tpgames.org/builder_green4_example.webm">CREATE</a>: Create your powerful message for your teleporter with the tool
<li><a style="font-size:large;" href="http://tpgames.org/Releasing_codegen.txt">CODEGEN</a>: You get piece of c++-like code representing animation
<li><a style="font-size:large;" href="http://tpgames.org/Releasing_animations.txt">PUBLISH</a>: place 3d engine to your web server
<li><a style="font-size:large;" href="http://meshpage.org/gameapi_example.html">ENJOY</a>: Then open the animation in your browser
</ul>
<h2>What are the advantages in your approach?</h2>
<ul>
<li>OpenGL based software-only solution, so no need to buy a camera
<li>File size is very small. The file size is like 30 lines of c++ code,
instead of megabytes of video files
<li>It's based on "designing the future", instead of "recording the past"
<li>Teleportation works via URL's: normal web technologies are used to transfer animations to other computers on the planet 
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
<li>We don't have sound or music support at all, because it's fundamentally based on recording industry technology, and we're not experts in that area.
</ul>
<h2>How is this different from other user-generated-content sites?</h2>
<ul>
<li>There's isn't any issues with copyrights -- i.e. no DCMA kind of problems (this is necessary because it must work in EU)
<li>All content for the site must be created with the builder tool
<li>The tool can control what content is allowed to be included to the product, thus we can check against copyright infringements
<li>Bm_url and FI_load are the biggest features in this area.
<p>
<li>If we don't like some content, we can "hide" it from the front page, but
    the urls still continue to work
</ul>
<h2>How does meshpage.org get licenses to user's content?</h2>
<ul>
<li>User still needs to agree to license the content he created with the tool
to meshpage.org and associates
<li>This is similar to how Youtube handles it, i.e. when user presses "Save & Publish" button on the web page, we get a license to display the content on our web page - before that, it'll only be displayed for that same user who is posting the content
<li>This is basically "implicit licensing".
<li>But current situation is that users are spending much less time creating content than what it took to create the GameApi library and GameApi Builder tool, so it can still be considered exchange of licenses -- user can use the builder tool in exchange of giving license to the content created with the tool.
</ul>
<h2>What other problems are you seeing currently?</h2>
<ul>
<li>Large problem in today's technologies is something called "Effort calculation".
<li>Effort calculation is based on the premise that web site authors are eating significant amount of time from their user base, but cannot even say critical questions like:
  <ul>
  <li>how much time they're eating from their user base?
  <li>how many people are using their site?
  <li>how they can ensure that work amounts imposed to the user base are staying within acceptable range?
  <li>what actions are taken, if work amount explodes and people are doing too much work to improve the user-generated-content-site?
  </ul>
This kind of basic information is usually missing, and "Effort Calculation" is the key metric how it should be handled.
</ul>
<h2>How do bm_url and FI_load work?</h2>

You might notice that bm_url and FI_load are refusing to load some
urls which are not around the user's homepage.<p>

We have some technology included that checks against user's copyright
infringements, before posting the content to meshpage.org. This is
implemented in the bm_url and FI_load features. Basically we use user's
homepage as a center of user's world, and implement a domain
restriction which limits usage of content inside meshpage.org
animations. For example when it loads png image from some url, it
checks whether the user's homepage was in the same domain.<p>

The theory behind this check is that we should allow only content that
user published himself, instead of all the content in the world, and
thus domain where user's homepage is at, is good candidate for allowed
content. This cannot provide absolute guarantee against copyright
infringements, but it prevents certain kind of spread pattern where
user picks urls from all around the world and uses them as content
inside the animations. Since meshpage.org is publishing this content
in the web page, we try to check problems in the content.<p>

This domain restriction check is more cunning that you'd expect, since if
you use several pieces of content in an animation, they need to all come from
the same domain. I.e. you can't pick one url from one domain and next url from
completely different place. Expectation is that animations contain more than
one piece of content/sprites/bitmaps/fonts/etc.., and thus the domain restriction can pinpoint the location of the user's real homepage address.<p>

Other features than bm_url and FI_load cannot use external content at
all, so all content coming from other features are built competely
using GameApi Builder tool. This ensures pretty much that the user
created it himself, at least if copy-pasting CodeGen output code snippets
are not counted.<p>

Note that currently we do not have login system for end users
implemented, so the domain check is per individual animation. Once
login system gets improved, it would allow implementing the domain
check for per-user homepage, instead of per-animation homepage. This
would improve the system, but is not currently implemented.

<h2>What features have domain check like bm_url & FI_load</h2>
<ul>
<li>bm_url
<li>p_url
<li>p_mtl
<li>p_mtl_url
<li>gltf
<li>newfont
<li>fi_load / load_font
<li>load_song
</ul>
This means these can load asyncronously data from url's and that
loading is handled via domain restriction.

<h2>Why builder can't load textures via p_mtl?</h2>
<ul>
<li>it can, but you need to use p_mtl and m_texture_many_p together.
</ul>

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
<h2>I get error while loading the animations</h2>
<ul>
<li>With new pthreads support in the site, browser settings need to be changed
<li>Chrome users: chrome://flags should have webassembly pthreads enabled
<li>Firefox users: about:config shuold have shared.memory=true
</ul>
<h2>What browser should I use?</h2>
On my laptop I get the following benchmarks(this test: <a href="https://meshpage.org/mesh_display?id=GVILK@032">here</a>(
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
<p>
<p>
<h1>builder tool download</h1>
<?php
require_once("user.php");
?>
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; background-color: white;">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi Builder</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">Modelling Tool, Gamedev</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Windows 10 64-bit</span>
<li><b>Download url:</b> <a href="https://tpgames.org/GameApi-Builder-v25.msi">download msi</a>
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
<p>
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; background-color: white;">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi Builder</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">Modelling Tool, Gamedev</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Linux 64-bit amd64</span>
<li><b>Download url:</b> <a href="https://tpgames.org/gameapi-builder_1.0-25.deb">download deb</a>
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
<p>
<div itemscope itemtype="http://schema.org/SoftwareApplication" style="border-style: solid; width: 400px; background-color: white;">
<ul>
<li><b>Application name:</b> <span itemprop="name">GameApi Builder</span>
<li><b>Application category:</b> <span itemprop="applicationCategory" itemtype="http://schema.org/SoftwareApplication">Modelling Tool, Gamedev</span>
<li><b>Operating system:</b> <span itemprop="operatingSystem">Web</span>
<li><b>Tool url:</b> <a href="https://meshpage.org/gameapi_builder.php">execute tool in web browser</a>
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
<p>
<link itemprop="applicationCategory" href="http://schema.org/ModellingTool">
<a itemprop="downloadUrl" href="https://tpgames.org/GameApi-Builder-v25.msi">
<img src="https://tpgames.org/gameapi-builder-screenshot2.png" width="901" height="199" crossorigin></a>

<div>
<a href="https://tpgames.org/builder_screenshot.webp" target=_blank><img src="https://tpgames.org/builder_screenshot.webp" width="300" height="200" crossorigin></img></a>
<a href="https://tpgames.org/builder_screenshot2.webp" target=_blank><img src="https://tpgames.org/builder_screenshot2.webp" width="300" height="200" crossorigin></img></a>
<a href="https://tpgames.org/builder_screenshot3.webp" target=_blank><img src="https://tpgames.org/builder_screenshot3.webp" width="300" height="200" crossorigin></img></a>
</div>
</div>


</div>
</div>

</div> <!-- app.. vue ends here -->
</body>
<style>
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
  background-color: #000000;
  margin: 0;
  padding: 0;
  width: 820px;
  height: 620px;
  }
</style>
<script>
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
    html_embed: false
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


var app = new Vue({
   el: '#app',
   mounted: function() {
          var vm = this;
     choose_breadlist(0,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
     start_emscripten(vm);
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
       var n = url.search("p=");
       var n2 = url.search("page=");
       var cut = 2;
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
	  start_emscripten(vm);
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
      echo "setTimeout(delayed_choose_display(vm,g_last_id,g_last_label),100);";
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
   methods: {

       mesh_display(id,label) {
          var vm = this;
          if (!g_emscripten_alive) { start_timer(id,label,vm); return; }
	  choose_breadlist(1,vm.main_breadcrumb,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
	  choose_display(id,label,vm,false);

	  //console.log(id);
       },
       bread_click(e) {
       		      var vm = this;
		      var txt = e.target.textContent;
       		      //console.log(txt);
		      choose_breadcrumb(txt,vm.main_breadcrumb,store,vm.main_breadcrumb_first,vm.main_breadcrumb_second);
    		      },

   },
   data: {
     state: store.state,
     main_breadcrumb_second: [{num:0,name: "mesh_all", choose:"main", title:"meshpage.org", link:false},
     {num:1,name:"mesh_display",choose:"mesh",title:"mesh display", link:false},
     ],
     main_breadcrumb_first: [{num:0,name: "mesh_all", choose:"main", title:"meshpage.org", link:true},
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
		       indicator: [true],
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
       g_tm_cb = window.setTimeout(timer_timeout(id,label,vm), 100);
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
	     window.history.replaceState({page: 2},"title 2", "?p=" + (i+3).toString());
	     } else {
	     if (i==0) {
	     window.history.back();
//window.history.replaceState({page: 2},"title 2", "meshpage?p=" + (i+1).toString());
	     }
	     }

      } else {
        bread.link = false;
      }
   }

      if (txt=='meshpage.org') {
          choose_breadlist(0,breadcrumb,first,second);
	  if (typeof fix_keyboard === "function") fix_keyboard(true);
	  }
      if (txt!='mesh') {
           var elem = document.getElementById("display");
	   elem.style.display = "none";
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


function choose_display(id,label, vm,is_popstate)
{
  if (!is_popstate) {
  g_last_id = id;
  g_last_label = label;
  	       // ?p=2&id="
  window.history.pushState({page: 2},"title 2", "/" + id.toString() ); // + "&label=" + label.toString()
  }
  store.choose("mesh");
  choose_breadcrumb("mesh display",vm.main_breadcrumb,store,vm.main_breadcrumb_first,vm.main_breadcrumb_second);

  var url = "https://meshpage.org/mesh_pre.php?id=" + label;
  var url2 = "https://meshpage.org/mesh_addtext.php?id=" + label;
  var url3 = "https://meshpage.org/mesh_background.php?id=" + label;
  //console.log(g_txt[id]);
  if (g_txt[id]===undefined) {

const myHeaders3 = new Headers();
const myBRequest = new Request(url3, {
   method: 'GET',
   headers: myHeaders3,
   mode: 'same-origin',
   cache: 'default'
});
fetch(myBRequest).then((r) => {
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
  setTimeout(function() { start_emscripten_really(vm) },100);
}

var g_emscripten_alive = false;
function check_em(indicator) {
  return function() {
	       g_emscripten_alive=true;
	       indicator.pop();
	       indicator.push(false);
	       return;
	       }
}
function check_emscripten_running(indicator)
{
  var canv = document.getElementById("canvas");
  if (Module) {
	 Module['onRuntimeInitialized'] = check_em(indicator);
   	 //console.log("RUNNING");
	 } else {
   	 //console.log("TIMEOUT");
	 setTimeout(function() { check_emscripten_running(indicator) },100);
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
   setTimeout(function() { check_emscripten_running(indicator) },100);
}


function resize_size() {
  return [830,630];
}

var gameapi_homepageurl = "http://tpgames.org";

var g_display_timeout = null;
function display_cb()
{
              var elem = document.getElementById("display");
	      elem.style.display = "block";
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
   }
}

function show_emscripten(str,hide,indicator,is_async)
{
	if (typeof fix_keyboard === "function") fix_keyboard(hide);

	g_emscripten_start_count++;
	if (g_emscripten_running) {
	   str+="\n";
	   try {
	       if (is_async) {
	       	  Module.ccall('set_string', null, ['number', 'string'],[0,str],{async:true});
		  Module.ccall('set_background_mode', null, ['number'], [g_background], {async:true});
		  } else {
	       	  Module.ccall('set_string', null, ['number', 'string'],[0,str]);
		  Module.ccall('set_background_mode', null, ['number'], [g_background]);
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
  var Module = {
      canvas : canv,
      arguments : [
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

      "--code", "P I1=ev.polygon_api.p_empty();\nML I2=ev.polygon_api.render_vertex_array_ml2(ev,I1);\nRUN I3=ev.blocker_api.game_window2(ev,I2,false,false,0.0,100000.0);\n", "--homepage",gameapi_homepageurl,"--platform","<?php echo $ua ?>"],
      print: (function() {
      	     return function(text) {
	     	    console.log(text);
		    } })(),      
  };
</script>
<?php
require_once("user.php");
$nothreads = js_no_threads();
$mobile = js_mobile();
if ($mobile == "yes") {
  echo "<script src='web_page_lowmem_nothreads.js?" . filemtime("web_page_lowmem_nothreads.js") . "'></script>";
} else
if ($nothreads == "yes") {
  echo "<script src='web_page_nothreads.js?" . filemtime("web_page_nothreads.js") . "'></script>";
} else {
  echo "<script src='web_page.js?" . filemtime("web_page.js") . "' crossorigin='anonymous'></script>";
}
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
  echo "hd-=180;";
  }
?>
  var elem = document.getElementById("display");
  if (typeof(elem)!='undefined' && elem != null) { } else { return; }
  elem.style.width = (wd).toString() + "px";
  elem.style.height = (hd).toString() + "px";

  var iframe = document.getElementById("canvas");
  if (!iframe) return;
  iframe.style.width = (wd).toString() + "px";
  iframe.style.height = (hd).toString() + "px";

  //var prog = document.getElementById("prgress");
  //prog.style.width = (wd).toString() + "px";
  //var hdd = 30*hd3;
//prog.style.height = hdd.toString() + "px";

  if (g_emscripten_alive && g_emscripten_running && Module) {
	   try {
Module.ccall('set_resize_event', null, ['number', 'number'], [wd,hd], {async:true});
	   } catch(e) {
	     console.log(e);
	   }

  }

}
</script>
<script>
function connect() {
   var connection = new RTCPeerConnection({
       iceServers: [
         { 'url': 'stun:stun.1.46.32.252.120:424242' }]});
   const signalingChannel = new SignalingChannel(connection);
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
</script>