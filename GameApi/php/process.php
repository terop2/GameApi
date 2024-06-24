<?php

$labels = array("3D Model Viewer", "GLTF to HTML5 ZIP", "Full Customisation", "Examples");
$imgs = array("img_1.png", "img_2.png", "img_3.png", "img_4.png");
$links = array("view.php", "gltf_to_zip.php", "meshpage_4", "meshpage.php");


echo "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
echo "<html>";
echo "<h1 align=\"center\" style=\"font-size: 80px;\">meshpage.org</h1>";
echo "<h4 align=\"center\" style=\"position: relative; top: -55px; font-size: 14px;\">(A way to display your 3d models in web)</h4>";
echo "<div class=\"grid_container\">";
for($i=0;$i<4;$i++)
{
   echo "<div class=\"grid_item\">";
   echo "<a class=\"link\" href=\"$links[$i]\">";
   echo "<div class=\"grid_elem\">";
   echo "<div class=\"grid_item1 width320\" style=\"font-size: 60px;\">";
   echo "$labels[$i]";
   echo "</div>";
   echo "<div class=\"grid_item2 center width150\">";
   echo "<img src=\"$imgs[$i]\" width=\"150\" height=\"110\"\/>";
   echo "</div>";
   echo "</div>";
   echo "</a>";
   echo "</div>";

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
echo "  background-color: ff8822;";
echo "}";
echo ".grid_elem {";
echo "  display: grid;";
echo "   margin: 5px 0px 5px 5px;";
echo "}";
echo ".grid_item1 {";
echo "  display: flex;";
echo "  grid-column: 1/2;";
echo "   text-align: center;";
echo "   justify-content: center;";
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
echo ".center {";
echo "  margin: auto; width: 150;";
echo "}";
echo "}";
echo ".width320 {";
echo "  width: 580px;";
echo "}";
echo ".width150 {";
echo "  width: 150px;";
echo "}";


echo ".grid_item2 {";
echo "  grid-column: 2/2;";
echo "   margin: 5px 5px 5px 0px;";
echo "   padding: 5px; ";
echo "}";
echo ".grid_item:hover {";
echo "   background-color: 2288ff;";
echo "}";
echo "</style>";
echo "</html>";