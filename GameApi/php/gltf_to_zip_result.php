<?php

$gltf = $_POST["gltffile"];

if (strlen($gltf)<4) {
  echo "<pre>";
  echo "ERROR, SOMETHING WRONG WITH THE URL YOU ENTERED</pre>";
} else {


$transparent = $_POST["transparent"];
$zoom = $_POST["zoom"];
$rotate = $_POST["rotate"];
$pan = $_POST["pan"];

$file = "TF I1=ev.mainloop_api.gltf_loadKK2(" . $gltf . ");\n";
$file .= "ML I2=ev.mainloop_api.gltf_mesh_all(ev,I1,1.0,0);\n";

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

$file .= "ML I7=ev.mainloop_api.async_gltf(I6,I1);\n";
$file .= "RUN I8=ev.blocker_api.game_window2(ev,I7,false,false,0.0,100000.0);\n";

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
header("Content-Disposition: attachment; filename=\"deploy.zip\"");
echo "$file2";
} else {
  echo "<pre>$file3";
  echo "THERE SEEMS TO BE ERRORS!</pre>";
}
}