<?php

$labels = array("3D Model Viewer", "GLTF to HTML5 ZIP", "Full Customisation", "Examples");
$imgs = array("img_1.webp", "img_2.webp", "img_3.webp", "img_4.webp");
$alts = array("3d model viewer", "gltf to html5 zip converter", "builder tool download", "3d design examples");
$links = array("view.php", "gltf_to_zip.php", "meshpage_4", "meshpage.php");
$descs = array("quickly test that your <br>gltf files are<br>compatible<br> with the engine",
       	       "convert your<br>gltf files to html5<br>for hosting space<br>deployment",
	       "if end result needed<br>tweaking, use full<br> customisation",
	       "and there's plenty<br> of examples to learn<br> the scripting language");


echo "<!DOCTYPE html>";
echo "<html lang=\"en\">";
echo "<meta name=\"description\" content=\"Every web page on the planet needs to be converted from plain 2d to rich and powerful 3d format, now with our 3d tools the conversion is easier than ever. We just need to dip into your gltf file collection.\">";
echo "<title>A way to display your 3d models on the web</title>";
echo "<head>";
echo "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
echo "</head>";
echo "<body>";
echo "<h1 class=\"customfont\" align=\"center\" style=\"font-size: 80px;\">meshpage.org</h1>";
echo "<h4 class=\"customfont\" align=\"center\" style=\"position: relative; top: -55px; font-size: 14px;\">(A way to display your 3d models on the web)</h4>";
echo "<div class=\"grid_container\">";
for($i=0;$i<4;$i++)
{
   echo "<div class=\"grid_item\">";
   echo "<a class=\"link\" href=\"$links[$i]\">";
   echo "<div class=\"grid_elem\">";
   echo "<div class=\"grid_item1 width320 customfont\" style=\"font-size: 40px;\">";
   echo "<strong>$labels[$i]</strong>";
   echo "</div>";
   echo "<div class=\"grid_item2 center width150\">";
   echo "<img src=\"$imgs[$i]\" width=\"150\" height=\"110\" alt=\"$alts[$i]\"\/>";
   echo "</div>";

if (substr_count($descs[$i],"<br>")>=3) {
   echo "<div class=\"grid_item3 width200 customfont\" style=\"font-size: 20px; margin: 15px 5px 5px 0px\">";
   } else {
   echo "<div class=\"grid_item3 width200 customfont\" style=\"font-size: 20px; margin: 25px 5px 5px 0px\">";
   }
   echo "$descs[$i]";
   echo "</div>"; // grid item3
   echo "</div>"; // grid_elem
   echo "</a>";
   echo "</div>"; // grid_item

}
echo "</div>";
echo "<style>";
echo "a.link {";
echo "   text-decoration: none !important;";
echo "   -webkit-box-shadow: none !important;";
echo "   box-shadow: none !important;";
echo "}";
echo ".grid_container {";
echo "   display: grid;";
echo "   margin: 20px; ";
echo "}";
echo ".grid_item {";
echo "  display: grid;";
echo "   margin: 5px 0px 5px 5px;";
echo "  background-color: #aa9988;";
echo "}";
echo ".grid_elem {";
echo "  display: grid;";
echo "  grid-auto-flow: column;";
echo "   margin: 5px 0px 5px 5px;";
echo "}";
echo ".grid_item1 {";
echo "  display: flex;";
echo "  grid-column: 1/2;";
echo "   text-align: right;";
echo "   justify-content: right;";
echo "   text-decoration-line: none;";
echo "   color: black;";
echo "   padding: 28px 0px 0px 0px;";
echo "}";
echo "@media only screen and (max-width: 600px) {";
echo ".grid_item1 {";
echo "  width: 0px;";
echo "  display: none;";
echo "}";
echo ".grid_container > div {";
echo "   align-items: center;";
echo "   align-content: center;";
echo "}";
echo ".grid_elem > div {";
echo "   align-items: center;";
echo "   align-content: center;";
echo "}";
echo ".center {";
echo "  margin: auto; width: 150;";
echo "}";
echo "}";

echo ".width320 {";
echo "  width: 380px;";
echo "}";
echo ".width150 {";
echo "  width: 150px;";
echo "}";
echo ".width200 {";
echo "  width: 200px;";
echo "}";


echo ".grid_item2 {";
echo "  grid-column: 2/3;";
echo "   margin: 5px 5px 5px 0px;";
echo "   padding: 5px; ";
echo "}";
echo ".grid_item3 {";
echo "  display: block;";
echo "  grid-column: 3/3;";
//echo "   margin: 25px 5px 5px 0px;";
echo "   padding: 5px; ";
echo "   text-align: left;";
echo "   justify-content: center;";
echo "   justify-items: center;";
echo "   text-decoration-line: none;";
echo "   color: black;";
echo "   height: 75px;";
echo "}";
echo "@media only screen and (max-width: 780px) {";
echo ".grid_item3 {";
echo " width:0px; display:none;";
echo "}";
echo "}";

echo ".grid_item:hover {";
echo "   background-color: #ffaa88;";
echo "}";
echo "@font-face { font-family: \"custom\"; src: url(\"cafe.ttf\"); }";
echo ".customfont { font-family: custom; }";
echo "</style>";


echo "<!-- Begin SF Tag -->";
echo "<div class=\"sf-root\" data-id=\"3771512\" data-badge=\"light-default\" data-variant-id=\"sf\" style=\"width:125px\">";
echo "<a href=\"https://sourceforge.net/software/product/Meshpage.org/\" target=\"_blank\">Meshpage.org Reviews</a>";
echo "</div>";
echo "<script>(function () {var sc=document.createElement('script');sc.async=true;sc.src='https://b.sf-syn.com/badge_js?sf_id=3771512&variant_id=sf';var p=document.getElementsByTagName('script')[0];p.parentNode.insertBefore(sc, p);})();";
echo "</script>";
echo "<!-- End SF Tag -->";
echo "</body>";
echo "</html>";