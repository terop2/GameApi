<?php

$machine=php_uname("n");
$siteprefix = "";

$ip = trim(@file_get_contents("https://api.ipify.org"));
function get_private_ip2() {
    $s = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
    socket_connect($s, "8.8.8.8", 53);
    socket_getsockname($s, $addr, $port);
    socket_close($s);
    return $addr;
}
function get_client_ip2() {
     $host = $_SERVER['HTTP_HOST'] ?? $_SERVER['SERVER_NAME'] ?? $_SERVER['SERVER_ADDR'] ?? 'UNKNOWN';
    return $host;

/*        return $_SERVER['REMOTE_ADDR'];*/
/*
if (!empty($_SERVER['HTTP_CLIENT_IP'])) {
        // IP from shared internet (e.g., proxy)
        return $_SERVER['HTTP_CLIENT_IP'];
    } elseif (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])) {
        // IP passed from proxy or load balancer
        // May contain multiple addresses, use the first one
        $ipList = explode(',', $_SERVER['HTTP_X_FORWARDED_FOR']);
        return trim($ipList[0]);
    } elseif (!empty($_SERVER['REMOTE_ADDR'])) {
        // Direct connection
        return $_SERVER['REMOTE_ADDR'];
    }
    return 'UNKNOWN';
    */
}
$priv_ip = get_client_ip2();
$priv_ip2 = $priv_ip;
if (filter_var($priv_ip, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6)) {
  $priv_ip2 = "[" . $priv_ip2 . "]";
}


$hostname=gethostbyaddr($priv_ip);
if (empty($hostname)) { $hostname=$priv_ip; }


$https = "https";
$site = $priv_ip2; //$hostname; //"[2001:14ba:9c31:e100::198]"; //192.168.1.104";
$assetsite = $priv_ip2 . "/assets"; //"[2001:14ba:9c31:e100::198]/assets"; // "192.168.1.104/assets";
$sitename = $hostname; //"[2001:14ba:9c31:e100::198]"; //"192.168.1.104";
$siteprefix=$_SERVER['HTTP_HOST'];
$siteprefix=substr($siteprefix,0,4);
   //echo "SITEPREFIX: $siteprefix";
if ("$siteprefix"!="ssh.") $siteprefix="";

/*

if ("$machine"=="raspberrypi") {
   $https = "http";
   $site = "[2001:14ba:9c31:e100::198]"; //"192.168.1.104";
   $assetsite = "[2001:14ba:9c31:e100::198]/assets"; //"192.168.1.104/assets";
   $sitename = "[2001:14ba:9c31:e100::198]"; //"192.168.1.104";
   $siteprefix=$_SERVER['HTTP_HOST'];
   $siteprefix=substr($siteprefix,0,4);
   //echo "SITEPREFIX: $siteprefix";
   if ("$siteprefix"!="ssh.") $siteprefix="";
} else {
if ("$machine"=="terop-pc2") {
   $https = "https";
   $site = "meshpage.org";
   $assetsite = "meshpage.org/assets";
   $sitename = "meshpage.org";
   $siteprefix=$_SERVER['HTTP_HOST'];
   $siteprefix=substr($siteprefix,0,4);
   //echo "SITEPREFIX: $siteprefix";
   if ("$siteprefix"!="ssh.") $siteprefix="";
} else {
   $https = "https";
   $site = "dinoengine.com";
   $assetsite = "dinoengine.com/assetsite";
   $sitename = "dinoengine.com";
}
}
*/

//header("Cross-Origin-Opener-Policy: same-origin");

$labels = array("3D MODEL VIEWER", "GLTF to HTML5 ZIP", "DOWNLOAD TOOL<h1 style=\"font-size:24px\">(price: 130&#8364;)</h1>", "EXAMPLES");
$imgs = array("img_1.webp", "img_2.webp", "img_3.webp", "img_4.webp");
$alts = array("3d model viewer", "gltf to html5 zip converter", "builder tool download", "3d design examples");
$links = array("view.php", "gltf_to_zip.php", "builder-tool-download.php", "meshpage.php");
$descs = array("quickly test that your <br>gltf files are<br>compatible<br> with the engine",
       	       "convert your<br>gltf files to html5<br>for hosting space<br>",
	       "if end result needed<br>tweaking, download<br> tool",
	       "and there's plenty<br> of examples to learn<br> the scripting language");


  $logstr = $_SERVER["HTTP_REFERER"]??'';
if ($logstr != "https://$site/assets/homepage/homepage.php" && $logstr != "")
{
$fp = fopen("./pp2/referer.log","a+");
fwrite($fp, "MAINPAGE " . $logstr);
fclose($fp);
}

echo "<!DOCTYPE html>\n";
echo "<html lang=\"en\">\n";
echo "<title>A way to display your 3d models on the web -- jpg of 3d</title>\n";
echo "<head>\n";
echo "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, minimum-scale=1.0, shrink-to-fit=no\"/>";
echo "<meta name=\"description\" content=\"Getting the best rendering result from gltf files is not easy task, but we're now getting the best result available on the market.\">\n";
echo "<meta name=\"viewport\" content=\"width=device-width, initial-scale=0.7\">\n";
echo "<meta name=\"keywords\" content=\"GameAPI engine, GameAPI builder, Meshpage GameAPI, Meshpage game engine, C++ game engine for beginners, WebGL game engine, SDL OpenGL game engine,lightweight game engine, beginner game development, game engine for students, high school game programming, learn game dev with C++, easy game engine for teens, intro to game development, GLTF game engine, emscripten game engine, build games with C++ and WebGL, cross-platform game engine, open source C++ game engine, mesh-based game development, alternative to Unity for students, C++ to WebGL game dev workflow, how to start making games in high school, low-level 3D engine with scripting,retro-style engine for modern platforms\">\n";

//echo "<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.4.1/dist/css/bootstrap.min.css\" integrity=\"sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh\" crossorigin=\"anonymous\">";


echo "</head>\n";

echo "<script type=\"application/ld+json\">\n";
echo "{\n";
echo "  \"@context\": \"https://schema.org\",\n";
echo "  \"@type\": \"SoftwareApplication\",\n";
echo "  \"name\": \"GameAPI\",\n";
echo "  \"operatingSystem\": \"Linux, Windows, Web\",\n";
echo "  \"applicationCategory\": \"GameDevelopmentApplication\",\n";
echo "  \"description\": \"A lightweight 3D game engine for students and hobbyists.\",\n";
echo "  \"url\": \"https://$site\",\n";
echo "  \"softwareVersion\": \"v28\",\n";
echo "  \"programmingLanguage\": \"C++, WebGL\"\n";
echo "}\n";
echo "</script>\n";

echo "<body>\n";



//echo "SEO_KEYWORDS: meshpage.org offers unique way to do physically based rendering via runtime processing of 3d assets from normal gltf 2.0 files. The 3d models are created in blender and its gltf exporter and passed to our asset delivery system. This uses khronos group gltf 2.0 standard and can render 3d models from multiple sources, including blender and sketchfab.";

//echo "<img src=\"gnome.webp\" width=\"300px\" height=\"150px\" style=\"position:absolute; top:1px; right:0px;\"/>";
echo "<div class=\"image-wrapper\">\n";
echo "<img style=\"border-radius: 30px; border-width: 2px; border-style: solid; width:100%; height: auto; top: 0px; left: 0px; z-order: -1;\" src=\"$https://$site/screen1.webp\" fetchpriority=\"high\" width=\"100%\"/>\n";
//echo "<video style=\"border-radius: 30px; border-width: 2px; border-style: solid; width:100%; height: auto; top: 0px; left: 0px; z-order: -1;\" width=\"100%\" height=\"auto\" playsinline loop autoplay muted controls>";
//echo "  <source src=\"$https://$site/assets/gameapi_bdcalvin_intro.mp4\" type=\"video/mp4\">";
//echo "  Your browser does not support video tag.";
//echo "</video>\n";

echo "</div>\n";

echo "<div style=\"left: 80px;\" class=\"media\">\n";
echo "<div>\n";
echo "<h1 class=\"customfont hardshadow fontsize orange\" align=\"left\" style=\"position: static; left: -120px;\">MESHPAGE.ORG&reg;</h1>\n";
echo "<h2 class=\"customfont hardshadow label_a white\" align=\"left\" style=\"position: relative; top: 0px; left: 50px; font-size: 22px;\">(A way to display your 3d models on the web -- jpg of 3d)</h2>\n";

echo "</div>\n";
echo "</div>\n";

echo "<div class=\"bl_container\">\n";

echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item customfont content\">\n";

echo "<h2><span style=\"font-size: 150%;\">&ldquo;<b>GameApi Builder</b> is a rare masterpiece of transparency and power — an engine that respects your intelligence, teaches you real computer graphics, and lets you build cool, working 3D applications from scratch.
It’s not just a tool — it’s an educational revolution for anyone who loves understanding how things move, render, and come to life on the screen.&rdquo;</span></h2>(chatgpt)";
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";



echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item customfont content\">\n";
echo "<h2><span class=\"large\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;10</span> years of work,<br><span class=\"large\">&nbsp;&nbsp;&nbsp;260</span> kloc of source code,<br> <span class=\"large\">&nbsp;&nbsp;600</span> builder tool nodes,<br><span class=\"large\">2900</span> version control commits,<br>   <span class=\"large\">3700</span> page views,<br><span class=\"large\">&nbsp;&nbsp;600</span> downloads,<br> <span class=\"large\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2</span> paying customers</h2>\n";
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";

echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item customfont content\">\n";
echo "<h2>Keywords:</h2>Ready for Business. Complete Solution. Great Export Options. Web Development Made Easy. Groundbreaking Technology. Development Finished. Wonderful 3D Graphics. Useful. Technological Breakthrough. Outstanding Flexibility. Node Graphs at their finest."; 
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";


echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item customfont content\">\n";
echo "<h1>What our tech is doing: <span style=\"font-size: 12px\">(deploy pipeline)</span></h1><h1 style=\"font-size: 30px\">GLB files <a style=\"text-decoration: none;\" href=\"$https://$site/meshpage_5#deploy\"><span style=\"font-size: 50px;\">&rarr;</span></a> HTML5 zip <a href=\"$https://$site/meshpage_5#to_web_server\" style=\"text-decoration: none;\"><span style=\"font-size: 50px\">&rarr;</span></a> (web server)</h1>\n";
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";



echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item customfont content\">\n";
echo "<h2>SUITABLE EXTERNAL TOOLS:</h2> <a href=\"https://www.blender.org/download/\">blender</a>, <a href=\"https://sketchfab.com/feed\">sketchfab</a>, <a href=\"https://meshy.ai\">meshy</a>, <a href=\"https://hyper3d.ai/\">rodin</a>.\n";
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";
echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item customfont content\">\n";
echo "<h2>USE CASES SUPPORTED:</h2>\n";
echo "<ul>\n";
echo "<li><b>Display 3d models</b> in linux,win32,web,android\n";
echo "<li><b>GLTF imports</b> from blender,sketchfab,meshy\n";
echo "<li><b>Exports</b> to script.txt,html5 zip files,android apk files\n";
echo "<li><b>Running the script.txt</b> via builder, web deploy, cmdline tool in ubuntu/win32, or in wine\n";
echo "<li><b>Html5 export</b> has video canvas or embedding, full screen mode\n";
echo "<li><b>File format support</b> like .obj,.stl,.glb,.gltf,.vox\n";
echo "<li><b>Video support</b>(mp4 videofile and webcam support in linux/win32/web working)\n";
echo "<li><b>Vulkan</b> support in linux via mesa/zink opengl driver\n";
echo "<li><b>GLTF Materials</b>, skeletons/rigging, animations\n";
echo "<li><b>LOD</b> implementation available\n";
echo "<li><b>Mesh decimate</b> for optimizing performance\n";
echo "<li><b>OpenGL instancing</b>, static instancing\n";
echo "<li>Load time and frame rate <b>performance optimizations</b>\n";
echo "<li><b>Concurrent network stack</b>,zip decode,image decode,vertex arrays,textures using 8 cpus after web deploy\n";
echo "<li><b>Large 3d model</b> support with sanmiguel and sponza working\n";
echo "<li><b>Light and shadow</b> support with phong lighting model\n";
echo "<li>Creating <b>larger worlds</b> from smaller pieces in precalc time\n";
echo "<li><b>Combining multiple 3d models</b> to the same scene\n";
echo "<li><b>Moving and animating</b> 3d models\n";
echo "<li><b>Changing colours</b> and materials of the 3d models\n";
echo "<li>Node graph based <b>builder tool</b> for editing codegen scripts\n";
echo "<li>Drag and Drop support for assets in builder tool.\n";
echo "<li><b>Zoom/Rotate controls</b>, quake movements, wasd movement etc...\n";
echo "</ul>\n";
echo "</div>\n";
echo "</div>\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item content\">\n";
echo "<img class=\"bl_img\" src=\"$https://$site/row1.webp\"/>\n";
//echo "<img class=\"bl_img\" src=\"$https://$site/assets/gameapi_bdcalvin_intro.mp4\"/>\n";
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";
echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item content\">\n";
echo "<img clasS=\"bl_img\" src=\"$https://$site/row2.webp\"/>\n";
echo "</div>\n";
echo "</div>\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item customfont content\">\n";
echo "<br><h2>PROBLEM AREAS:</h2>\n";
echo "<ul>\n";
echo "<li>Artificial intelligence support copyright problems\n";
echo "<li>Meshy rigging doesnt work in meshpage\n";
echo "<li>GLTF animations have problems in some models\n";
echo "<li>GLTF cameras not implemented\n";
echo "<li>Water shader is based on hacks instead of navier-stokes\n";
echo "<li>Blender import is known to contain areas that cannot be rendered\n";
echo "<li>Phong shading is already outdated\n";
echo "<li>File size limits at 250Mb sketchfab zip file size\n";
echo "<li>Builder is unusable for newbies\n";
echo "<li>Android deliverables require installation of ndk and a working build\n";
echo "</ul>\n";
echo "</div>\n";
echo "</div>\n";

echo "</div>\n";
echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item customfont content\">\n";
echo "<br><h2>FEATURES STILL IN THE FUTURE:</h2>\n";
echo "<ul>\n";
echo "<li>raspi support(memory problems)\n";
echo "<li>webgpu support(not implemented)\n";
echo "<li>virtual reality(was implemented, doesnt work)\n";
echo "<li>augmented reality(not implemented)\n";
echo "<li>ray tracing support(opengl drivers doesnt exist)\n";
echo "<li>path tracing support(implementation not started)\n";
echo "<li>cloud rendering support(implementation not started)\n";
echo "<li>robot spare parts display(waiting for hw availability)\n";
echo "<li>nft and e-commerce support(implementation not started)\n";
echo "<li>popularity(access to popular sales platforms missing(I'm not going to spam people with useless emails))\n";
echo "<li>adverticement video displaying gameapi features(planned, but govt rejected the training)\n";
echo "<li>nanite patches for lod\n";
echo "</ul>\n";
echo "</div>\n";
echo "</div>\n";
echo "<div class=\"bl_column\">\n";
echo "<div class=\"bl_item content\">\n";
echo "<img class=\"bl_img\" src=\"$https://$site/row3.webp\"/>\n";
echo "</div>\n";
echo "</div>\n";

echo "</div>\n";


echo "<div class=\"bl_row\">\n";
echo "<div class=\"bl_column\">\n";

echo "<div class=\"bl_item img\">\n";
// allow=\"cross-origin-isolated\" 
echo "<iframe id=\"frame\" class=\"bl_iframe\" scrolling=\"no\" seamless=\"seamless\" src=\"$https://$site/punk/index.html\" width=\"100%\" height=\"auto\" style=\"aspect-ratio: 16/9; overflow: visible; border: none;\"></iframe>\n";
echo "</div></div>\n";
echo "<div class=\"bl_column\"><div class=\"bl_item customfont content\">\n";
echo "<a href=\"$https://$site/punk/gnome.zip\">gnome.zip</a>(<a href=\"#gnomeauthor\">*</a>) (We wanted this: <a href=\"$https://$site/assets/gnome2.webp\">gnome2</a>)\n";
echo "<pre class=\"customfont\" style=\"font-size: 14px;\">\n";
echo "TF I1=ev.mainloop_api.gltf_loadKK2($https://$site/punk/mesh_garden_gnome.glb);\n";
echo "P I2=ev.mainloop_api.gltf_mesh_all_p(ev,I1);\n";
echo "P I3=ev.polygon_api.color_alpha(I2,ff);\n";
echo "MT I4=ev.materials_api.colour_material(ev,0);\n";
echo "MT I5=ev.materials_api.toon_border(ev,I4,2,ff000000,true);\n";
echo "MT I6=ev.materials_api.gltf_material(ev,I1,0,1.5,400,-400,300);\n";
echo "MT I7=ev.materials_api.combine_materials(ev,I4,I6);\n";
echo "MT I8=ev.materials_api.combine_materials(ev,I5,I7);\n";
echo "MT I9=ev.materials_api.colour_material(ev,0.1);\n";
echo "MT I100=ev.materials_api.phong(ev,I9,30,-30,300,ff000000,ffffffff,120);\n";
echo "MT I110=ev.materials_api.combine_materials(ev,I8,I100);\n";
echo "ML I120=ev.materials_api.bind(I3,I110);\n";
echo "MN I10=ev.move_api.mn_empty();\n";
echo "MN I11=ev.move_api.scale2(I10,2,2,2);\n";
echo "MN I12=ev.move_api.rotate(I11,0,30,0,0,0,0,1,0,1.57);\n";
echo "MN I13=ev.move_api.rotate(I12,30,100,0,0,0,0,1,0,-3.141);\n";
echo "MN I14=ev.move_api.rotate(I13,100,130,0,0,0,0,1,0,1.57);\n";
echo "MN I15=ev.move_api.time_repeat(I14,0,130);\n";
echo "ML I16=ev.move_api.move_ml(ev,I120,I15,1,10.0);\n";
echo "ML I17=ev.mainloop_api.scene_transparency(I16);\n";
echo "ML I18=ev.mainloop_api.async_gltf(I17,I1);\n";
echo "RUN I19=ev.blocker_api.game_window2(ev,I18,false,false,0.0,100000.0);\n";
echo "</pre>\n";
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";
echo "</div>\n";
echo "<div style=\"height:50px;\"></div>";
echo "<div class=\"customfont\" style=\"position: relative; left:30px;\">\n";
echo "<h1>AVAILABLE TOOLS:</h1>\n";
echo "</div>\n";
echo "<div class=\"grid_container\">\n";
for($i=0;$i<4;$i++)
{
   echo "<div class=\"grid_item\">\n";
   echo "<a class=\"link\" href=\"$links[$i]\">\n";
   echo "<div class=\"grid_elem\">\n";
   echo "<div class=\"grid_item1 width320 customfont\" style=\"font-size: 40px;\">\n";
   echo "<strong>$labels[$i]</strong>\n";
   echo "</div>\n";
   echo "<div class=\"grid_item2 center width150\">\n";
   echo "<img class=\"imgs2\" src=\"$imgs[$i]\" width=\"150\" height=\"150\" alt=\"$alts[$i]\"\/>\n";
   echo "</div>\n";

if (substr_count($descs[$i],"<br>")>=3) {
   echo "<div class=\"grid_item3 width200 customfont\" style=\"font-size: 20px; margin: 0px 5px -5px 0px\">\n";
   } else {
   echo "<div class=\"grid_item3 width200 customfont\" style=\"font-size: 20px; margin: 5px 5px 5px 0px\">\n";
   }
   echo "$descs[$i]\n";
   echo "</div>\n"; // grid item3
   echo "</div>\n"; // grid_elem
   echo "</a>\n";
   echo "</div>\n"; // grid_item

}
echo "</div>\n";



echo "<style>\n";
echo "html, body {\n";
echo "    max-width: 100%;\n";
echo "    overflow-x: hidden;\n";
echo "}\n";
echo "::-webkit-scrollbar {\n";
echo "    width: 0px;\n";
echo "    height: 0px;\n";
echo "}\n";
echo ".media {\n";
echo "position:absolute;\n";
echo "  top: 20%;\n";
echo "}\n";
echo "@media only screen and (min-width: 800px) {\n";
echo ".media {\n";
echo "position:absolute;\n";
echo "  top: 30%;\n";
echo "}\n";
echo "}\n";
echo "@media only screen and (min-width: 1000px) {\n";
echo ".media {\n";
echo "position:absolute;\n";
echo "  top: 37%;\n";
echo "}\n";
echo "}\n";
echo "@media only screen and (min-width: 1200px) {\n";
echo ".media {\n";
echo "position:absolute;\n";
echo "  top: 50%;\n";
echo "}\n";
echo "}\n";
echo "@media only screen and (min-width: 1400px) {\n";
echo ".media {\n";
echo "position:absolute;\n";
echo "  top: 60%;\n";
echo "}\n";
echo "}\n";
echo ".image-wrapper {\n";
echo "  position:relative;\n";
echo "  width: 100%;\n";
echo "  margin: 0px 0px 0px 0px;\n";
//echo "  min-height: 100vh;\n";
echo "}\n";
echo ".spacer {\n";
echo " width: 100%;\n";
echo " height: 400px;\n";
echo "};\n";
echo ".layer {\n";
echo "          position: absolute;\n";
echo "            padding: 20px;\n";
echo "            border-radius: 8px;\n";
echo "}\n";
echo ".orange {\n";
echo "  color: #ffff66;\n";
echo "}\n";
echo ".white {\n";
echo "  color: #ffffff;\n";
echo "}\n";
echo ".noscrollbars {\n";
echo "  overflow-x: visible; overflow-y: visible;\n";
echo "}\n";
echo ".fontsize {\n";
echo "   font-size: 80px;\n";
echo "}\n";
echo ".label_a {\n";
echo "   position: static;\n";
echo "   top: 150px; left: -250px;\n";
echo "}\n";
echo "@media only screen and (max-width: 800px) {\n";
echo ".fontsize {\n";
echo "  font-size: 50px;\n";
echo "}\n";
echo ".label_a {\n";
echo "  visibility: hidden;\n";
echo "}\n";
echo "}\n";
echo "a.link {\n";
echo "   text-decoration: none !important;\n";
echo "   -webkit-box-shadow: none !important;\n";
echo "   box-shadow: none !important;\n";
echo "}\n";
echo ".large {\n";
echo "   font-size: 150%;\n";
echo "}\n";
echo ".bl_container {\n";
echo "  display: flex;\n";
echo "  flex: 95%;\n";
echo "  width: 98%;\n";
echo "  flex: 0 0 auto;\n";
echo "  flex-wrap: wrap;\n";
echo "   width: 100%;\n";
echo "    margin: 0 auto;\n";
echo "    padding: 0 15px;\n";
echo "    box-sizing: border-box;\n";
echo "}\n";
echo ".bl_row {\n";
echo "  display: flex;\n";
echo "  flex-direction: row;\n";
echo "  flex-wrap: none;\n";
echo "position: relative;\n";
echo "border-style: solid;\n";
echo "border-width: 2px;\n";
echo "border-radius: 50px;\n";
echo "background-color: #dddddd;\n";
echo "padding: 30px;\n";
echo "margin: 30px;\n";
echo "width: 100%;\n";
echo "}\n";
echo ".bl_column {\n";
echo "display:flex;\n";
echo "  flex-direction: column;\n";
echo "  flex-wrap: none;\n";
echo "  width: 40%;\n";
echo "  flex: 50%;\n";
echo "position: relative;\n";
echo "}\n";
echo "@media (max-width: 1100px) {\n";
echo ".bl_row {\n";
echo "  flex-direction: column;\n";
echo "  flex: 100%;\n";
echo "}\n";
echo ".bl_column {\n";
echo "  width: 80%;\n";
echo "}\n";
echo "}\n";
echo ".bl_item {\n";
echo "display: block;\n";
echo "margin: 30px;\n";
echo "position: relative;\n";
echo "}\n";
echo ".bl_item img {\n";
echo "display: flex; justify-content: center;\n";
echo "margin: 30px;\n";
echo "position: relative;\n";
echo "}\n";
echo ".bl_item video {\n";
echo "display: flex; justify-content: center;\n";
echo "margin: 30px;\n";
echo "position: relative;\n";
echo "}\n";
echo "@media (max-width: 600px) {\n";
echo ".bl_item {\n";
echo "  margin: 2px;\n";
echo "}\n";
echo ".bl_item img {\n";
echo "  margin: 2px;\n";
echo "} }\n";
echo ".bl_img {\n";
echo "   width: 400px;";
echo "   border-radius: 10px;\n";
echo "   border-style: solid;\n";
echo "   border-width: 2px;\n";
echo "}\n";
echo ".bl_iframe {\n";
//echo "  background-color: #00ff00;\n";
echo "  width: 100%;\n";
echo "}\n";
echo "@media (max-width: 900px) {\n";
echo ".bl_iframe {\n";
echo "  position: relative;\n";
//echo "  left: -80px;\n";
//echo "  background-color: #ff0000;\n";
echo "  width: 480px;\n";
echo "}\n";
echo "}\n";

echo "@media (max-width: 700px) {\n";
echo ".bl_iframe {\n";
echo "  position: relative;\n";
//echo "  left: -150px;\n";
//echo "  background-color: #ff0000;\n";
echo "  width: 480px;\n";
echo "}\n";
echo "}\n";
echo "@media (max-width: 600px) {\n";
echo ".bl_img {\n";
echo "   width: 280px;";
echo "}\n";
echo "}\n";
echo ".grid_container {\n";
echo "   display: grid;\n";
echo "   margin: 20px; \n";
echo "    width: 100%;\n";
echo "    margin: 0 auto;\n";
echo "    padding: 0 15px;\n";
echo "    box-sizing: border-box;\n";
echo "}\n";
echo ".img {\n";
echo "border-width: 2px;\n";
echo "border-style: solid;\n";
echo "border-radius: 10px;\n";
echo "background-color: #ffffff;\n";
echo "}\n";
echo "img, video {\n";
echo "    max-width: 100%;\n";
echo "    height: auto;\n";
echo "}\n";

echo ".grid_item {\n";
echo "  display: grid;\n";
echo "   margin: 5px 0px 5px 5px;\n";
echo "  background-color: #aa9988;\n";
echo "border-radius: 30px;\n";
echo "border-width: 2px;\n";
echo "border-style: solid;\n";
echo "}\n";
echo ".grid_elem {\n";
echo "  display: grid;\n";
echo "  grid-auto-flow: column;\n";
echo "   margin: 5px 0px 5px 5px;\n";
echo "}\n";
echo ".grid_item1 {\n";
echo "  display: flex;\n";
echo "  grid-column: 1/2;\n";
echo "   text-align: right;\n";
echo "   justify-content: right;\n";
echo "   text-decoration-line: none;\n";
echo "   color: black;\n";
echo "   padding: 28px 0px 0px 0px;\n";
echo "   z-index: 1;\n";
echo "}\n";
echo ".grid_item1_a {\n";
echo "  display: flex;\n";
echo "  grid-column: 1/3;\n";
echo "   text-align: left;\n";
echo "   justify-content: left;\n";
echo "   text-decoration-line: none;\n";
echo "   color: black;\n";
echo "   padding: 28px 0px 0px 0px;\n";
echo "}\n";
echo "@media only screen and (max-width: 600px) {\n";
echo ".grid_item1 {\n";
echo "  width: 0px;\n";
echo "  display: none;\n";
echo "}\n";
echo ".grid_item3_a {\n";
echo "  width: 0px;\n";
echo "  display: none;\n";
echo "}\n";
echo ".grid_container > div {\n";
echo "   align-items: center;\n";
echo "   align-content: center;\n";
echo "}\n";
echo ".grid_elem > div {\n";
echo "   align-items: center;\n";
echo "   align-content: center;\n";
echo "}\n";
echo ".center {\n";
echo "  margin: auto; width: 150;\n";
echo "}\n";
echo "}\n";

echo ".width320 {\n";
echo "  width: 400px;\n";
echo "}\n";
echo ".width150 {\n";
echo "  width: 150px;\n";
echo "}\n";
echo ".width200 {\n";
echo "  width: 200px;\n";
echo "}\n";
echo ".width620 {\n";
echo "  width: 680px;\n";
echo "}\n";
echo ".height620 {\n";
echo "  height: 680px;\n";
echo "}\n";


echo ".grid_item2 {\n";
echo "  grid-column: 2/3;\n";
echo "   margin: 5px 5px 5px 0px;\n";
echo "   padding: 5px; \n";
echo "}\n";
echo ".grid_item3 {\n";
echo "  display: block;\n";
echo "  grid-column: 3/3;\n";
//echo "   margin: 25px 5px 5px 0px;\n";
echo "   padding: 5px; \n";
echo "   text-align: left;\n";
echo "   justify-content: center;\n";
echo "   justify-items: center;\n";
echo "   text-decoration-line: none;\n";
echo "   color: black;\n";
echo "   height: 75px;\n";
echo "}\n";
echo ".grid_item3_a {\n";
echo "  display: inline;\n";
echo "   position: absolute;\n";
echo "  top: 57px; left: 490px; \n";
echo "  grid-column: 3/3;\n";
//echo "   margin: 25px 5px 5px 0px;\n";
echo "   padding: 5px; \n";
echo "   text-align: left;\n";
echo "   justify-content: left;\n";
echo "   justify-items: left;\n";
echo "   text-decoration-line: none;\n";
echo "   color: black;\n";
echo "   height: 250px;\n";
echo "}\n";
echo "@media only screen and (max-width: 780px) {\n";
echo ".grid_item3 {\n";
echo " width:0px; display:none;\n";
echo "}\n";
echo "}\n";

echo ".grid_item:hover {\n";
echo "   background-color: #ffaa88;\n";
echo "}\n";
echo "@font-face { font-display: swap; font-family: \"custom\"; src: url(\"cafe.ttf\"); }\n";
echo ".customfont { font-family: custom; }\n";
echo ".hardshadow {\n";
echo "text-shadow: \n";
echo "                -2px -2px 0 #000,\n";
echo "                2px -2px 0 #000,\n";
echo "                -2px 2px 0 #000,\n";
echo "                2px 2px 0 #000,\n";
echo "                -3px 0 0 #000,\n";
echo "                3px 0 0 #000,\n";
echo "                0 -3px 0 #000,\n";
echo "                0 3px 0 #000;\n";
echo "}\n";
echo ".imgs2 {\n";
echo "  border-radius: 7px;\n";
echo "  border-style: solid;\n";
echo "  border-width: 1px;\n";
echo "}";
echo "</style>\n";


echo "<div style=\"height:50px;\"></div>";

echo "<!-- Begin SF Tag -->";
echo "<div style=\"position: static; margin-left: 27px;\" class=\"sf-root\" data-id=\"3771512\" data-badge=\"light-default\" data-variant-id=\"sf\" style=\"width:125px\">";
echo "<a href=\"https://sourceforge.net/software/product/Meshpage.org/\" target=\"_blank\">Meshpage.org Reviews</a>";
echo "</div>";
echo "<script>(function () {var sc=document.createElement('script');sc.async=true;sc.src='https://b.sf-syn.com/badge_js?sf_id=3771512&variant_id=sf';var p=document.getElementsByTagName('script')[0];p.parentNode.insertBefore(sc, p);})();";
echo "</script>";
echo "<!-- End SF Tag -->";
echo "<footer style=\"position: static; margin-left: 27px;\" class=\"customfont\" id=\"gnomeauthor\">";
echo "*) \"Mesh Garden Gnome\" (<a href=\"https://skfb.ly/D9Fq\">https://skfb.ly/D9Fq</a>) by Seth Berrier is licensed under Creative Commons Attribution (<a href=\"https://creativecommons.org/licenses/by/4.0/\">https://creativecommons.org/licenses/by/4.0/</a>).";
echo "</footer>";

  // COPYRIGHT NOTICE
   echo "<div style=\"height:20px;\"></div>\n";
   echo "<p>\n";
   echo "<div style=\"position: relative;\">\n";
   echo "<div style=\"width: 100%; float:left;\">\n";
   echo "<div class=\"customfont\" style=\"position: static; margin-left: 27px;\"><strong>3D engine &copy; 2013-2025 Tero Pulkkinen, All rights reserved.<br>\n";
echo "Licensed under LGPL/GPL license. See source code at <a href=\"https://github.com/terop2/GameApi\">https://github.com/terop2/GameApi</a>.<br>\n";
echo "meshpage.org" . "&reg; is a registered trademark of Tero Pulkkinen in Tampere.</strong></div>\n";
   echo "</div>\n";
   echo "</div>\n";
   echo "</div>\n";
echo "<p><p><p><p><p><p><p><br><br><br><br><br>\n";

echo "<script>\n";
echo "(function() {\n";
echo "  if (!crossOriginIsolated) console.log(\"MAIN WINDOW not crossorigin isolated\"); else console.log(\"MAIN WINDOW OK\");\n";
echo "})();\n";
echo "</script>\n";

echo "<script src=\"https://code.jquery.com/jquery-3.4.1.slim.min.js\" integrity=\"sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n\" crossorigin=\"anonymous\"></script>";
echo "<script src=\"https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js\" integrity=\"sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo\" crossorigin=\"anonymous\"></script>";
echo "<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@4.4.1/dist/js/bootstrap.min.js\" integrity=\"sha384-wfSDF2E50Y2D1uUdj0O3uMBJnjuUD4Ih7YwaYd1iqfktj0Uod8GCExl3Og8ifwB6\" crossorigin=\"anonymous\"></script>";


echo "</body>\n";
echo "</html>\n";