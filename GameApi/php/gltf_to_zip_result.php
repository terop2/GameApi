<?php
ini_set('memory_limit', '2048M');

$countname = "./pp2/tmp.count";
$num = file_get_contents($countname);
$num2 = intval($num) + 1;
$str = sprintf('%05d',$num2);
echo "$str";
file_put_contents($countname,$str);


$gltf = $_POST["gltffile"];

if (strlen($gltf)<4||(substr($gltf,-3)!="zip"&&substr($gltf,-3)!="glb"&&substr($gltf,-4)!="gltf")) {
  echo "<pre>";
  echo "ERROR, SOMETHING WRONG WITH THE URL YOU ENTERED</pre>";
  $logstr = $_POST["gltffile"] . " " . $transparent . " " . $zoom . " " . $rotate . " " . $pan . " " . $shadow . " " . $anim . " " . $bigscreen . " " . $sketchfab . "-> FAIL\n";
$fp = fopen("./pp2/tmp.log","a+");
fwrite($fp, $logstr);
fclose($fp);

} else {

$is_zip = "nope";
$ext = substr($gltf, -4);
if ($ext == ".zip")
{
  $is_zip = "yes";
}

$pos = strrpos($gltf, '/');
$gltf_filename = substr($gltf, $pos+1);

$pos2 = strrpos($gltf_filename,'.');
$without_ext = substr($gltf_filename,0,$pos2);
$gltf_filename = $without_ext . "_html5.zip";


$transparent = $_POST["transparent"];
$zoom = $_POST["zoom"];
$rotate = $_POST["rotate"];
//$pan = $_POST["pan"];
$pan = "nope";
$shadow = $_POST["shadow"];
$anim = $_POST["anim"];
$bigscreen = $_POST["bigscreen"];
$large = $_POST["large"];
$sketchfab = $_POST["sketchfab"];

if ($is_zip=="yes" && $sketchfab=="sketchfab")
   $file = "TF I1=ev.mainloop_api.gltf_load_sketchfab_zip(" . $gltf . ");\n";
else if ($sketchfab=="sketchfab")
   $file = "TF I1=ev.mainloop_api.gltf_loadKK2(" . $gltf . ");\n";
if ($is_zip=="yes" && $sketchfab!="sketchfab") {
   $file = "ML I8=ev.mainloop_api.load_zip(ev," . $gltf . ");\n";   
} else {
if ($shadow=="shadow") {
   if ($large!="large") {
      $file .= "P I20=ev.polygon_api.gltf_load(ev,I1,0,0);\n";
      } else {
      $file .= "P I20=ev.mainloop_api.gltf_mesh_all_p(ev,I1);\n";
      }
   $file .= "MT I210=ev.materials_api.gltf_material(ev,I1,0,1,-400,400,400);\n";
if ($anim=="anim") {
   $file .= "MT I21=ev.materials_api.gltf_anim_material2(ev,I1,0,230,I210,c,0);\n";

   $file .= "MT I202=ev.materials_api.m_def(ev);\n";
   $file .= "MT I212=ev.materials_api.gltf_anim_material2(ev,I1,0,230,I202,c,0);\n";

} else {
  $file .= "MT I21=ev.materials_api.mt_alt(ev,std::vector<MT>{I210},0);\n";
  $file .= "MT I212=ev.materials_api.m_def(ev);\n";
}
   $file .= "P I22=ev.polygon_api.cube(-300,300,-220,-200,-300,300);\n";
   $file .= "MT I23=ev.materials_api.colour_material(ev,0.5);\n";
   $file .= "ML I24=ev.materials_api.newshadow2_phong(ev,I20,I21,I22,I23,-0.3,-1,-0.3,0,1,ff884422,ffffffff,0.8,1024,false,I212);\n";
if ($anim=="anim") {
  if ($large!="large") {
  $file .= "P I201=ev.polygon_api.gltf_load(ev,I1,0,0);\n";
  $file .= "MT I211=ev.materials_api.gltf_material(ev,I1,0,1,-400,400,400);\n";
  $file .= "MT I221=ev.materials_api.gltf_anim_material2(ev,I1,0,230,I211,c,0);\n";
  $file .= "ML I25=ev.materials_api.bind(I201,I221);\n";
  } else {

  $file .= "ML I25=ev.mainloop_api.gltf_mesh_all_anim(ev,I1,1,0,c,-400,400,400,0.0,ff000000);\n";
 }

} else {
   $file .= "ML I25=ev.mainloop_api.gltf_mesh_all(ev,I1,1,0,-400,400,400,0.0,ff000000);\n";
}
if ($anim=="anim") {
   $file .= "ML I27=ev.mainloop_api.or_elem_ml(ev,I24,I25);\n";
   $file .= "ML I2=ev.mainloop_api.send_key_at_time(I27,0.0,99);\n";
} else {
   $file .= "ML I2=ev.mainloop_api.or_elem_ml(ev,I24,I25);\n";
}
} else
if ($anim=="anim") {
  if ($large!="large") {
  $file .= "P I20=ev.polygon_api.gltf_load(ev,I1,0,0);\n";
  $file .= "MT I21=ev.materials_api.gltf_material(ev,I1,0,1,-400,400,400);\n";
  $file .= "MT I22=ev.materials_api.gltf_anim_material2(ev,I1,0,230,I21,c,0);\n";
  $file .= "ML I23=ev.materials_api.bind(I20,I22);\n";
  $file .= "ML I2=ev.mainloop_api.send_key_at_time(I23,0.0,99);\n";
 } else {
  $file .= "ML I20=ev.mainloop_api.gltf_mesh_all_anim(ev,I1,1.0,0,c,-400,400,400,0.0,ff000000);\n";
  $file .= "ML I2=ev.mainloop_api.send_key_at_time(I20,0.0,99);\n";
  }
} else 
  $file .= "ML I2=ev.mainloop_api.gltf_mesh_all(ev,I1,1.0,0,-400,400,400,0.0,ff000000);\n";

if ($zoom=="zoom")
   $file .= "ML I3=ev.mainloop_api.mouse_roll_zoom2(ev,I2);\n";
else
   $file .= "ML I3=ev.mainloop_api.array_ml(ev,std::vector<ML>{I2});\n";

if ($rotate=="rotate")
   $file .= "ML I4=ev.mainloop_api.touch_rotate(ev,I3,true,true,0.01,0.01);\n";
else
   $file .= "ML I4=ev.mainloop_api.array_ml(ev,std::vector<ML>{I3});\n";

if ($pan=="pan")
   $file .= "ML I5=ev.mainloop_api.right_mouse_pan(ev,I4);\n";
else
   $file .= "ML I5=ev.mainloop_api.array_ml(ev,std::vector<ML>{I4});\n";

if ($transparent=="trans")
   $file .= "ML I6=ev.mainloop_api.scene_transparency(I5);\n";
else
   $file .= "ML I6=ev.mainloop_api.array_ml(ev,std::vector<ML>{I5});\n";

if ($bigscreen=="bigscreen") {
   $file .= "ML I70=ev.mainloop_api.fullscreen_button(ev);\n";
   $file .= "ML I7=ev.mainloop_api.array_ml(ev,std::vector<ML>{I6,I70});\n";
   }
else
   $file .= "ML I7=ev.mainloop_api.array_ml(ev,std::vector<ML>{I6});\n";

$file .= "ML I8=ev.mainloop_api.async_gltf(I7,I1);\n";
}
$file .= "RUN I9=ev.blocker_api.game_window2(ev,I8,false,false,0.0,100000.0);\n";

file_put_contents("./pp2/tmp.txt",$file);
file_put_contents("./pp2/tmp.zip","");

$urlsplit = parse_url($gltf);
$homepage = "https://" . $urlsplit["host"] . "/";

$cmd = "(cd ./pp2;LD_LIBRARY_PATH=.. nohup ../deploytool --file ./tmp.txt -o ./tmp.zip --homepage " . $homepage . " > test.txt 2>&1)";

shell_exec($cmd);


$file3 = file_get_contents("./pp2/test.txt");

$res = str_contains($file3,"ALL OK");

if ($res) {
$file2 = file_get_contents("./pp2/tmp.zip");
header("Content-Type: application/zip");
header("Content-Disposition: attachment; filename=\"$gltf_filename\"");
echo "$file2";

$logstr = $_POST["gltffile"] . " " . $transparent . " " . $zoom . " " . $rotate . " " . $pan . " " . $shadow . " " . $anim . " " . $bigscreen . " " . $sketchfab . " -> SUCCESS\n";
$fp = fopen("./pp2/tmp.log","a+");
fwrite($fp, $logstr);
fclose($fp);


} else {
  echo "<pre>$file3";
  echo "THERE SEEMS TO BE ERRORS!</pre>";

$logstr = $_POST["gltffile"] . " " . $transparent . " " . $zoom . " " . $rotate . " " . $pan . " " . $shadow . " " . $anim . " " . $bigscreen . " " . $sketchfab . "-> FAIL\n";
$fp = fopen("./pp2/tmp.log","a+");
fwrite($fp, $logstr);
fclose($fp);

}
}