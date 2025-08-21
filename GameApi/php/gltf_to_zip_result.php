<?php
ini_set('memory_limit', '2048M');


function getGoogleDriveFileInfo($fileId) {
    // Create metadata URL
    $metadataUrl = "https://drive.google.com/uc?id=" . $fileId . "&export=download";
    
    // Initialize cURL
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $metadataUrl);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_HEADER, true);
    curl_setopt($ch, CURLOPT_NOBODY, true); // HEAD request only
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    
    // Execute request
    $response = curl_exec($ch);
    
    // Parse headers
    $contentDisposition = '';
    $contentType = '';
    
    // Get headers
    $headerSize = curl_getinfo($ch, CURLINFO_HEADER_SIZE);
    $headers = substr($response, 0, $headerSize);

    // Parse Content-Disposition header
    if (preg_match('/[cC]ontent-[dD]isposition:.*filename="([^"]+)"/', $headers, $matches)) {
        $filename = $matches[1];
    } else if (preg_match('/[cC]ontent-[dD]isposition:.*filename\*=UTF-8\'\'(.+)/', $headers, $matches)) {
        $filename = urldecode($matches[1]);
    }
    
    // Get Content-Type
    if (preg_match('/Content-Type: (.+)/', $headers, $matches)) {
        $contentType = trim($matches[1]);
    }
    
    curl_close($ch);
    
    // Extract extension from filename
    $extension = '';
    if (!empty($filename)) {
        $extension = strtolower(pathinfo($filename, PATHINFO_EXTENSION));
    }
    
    // If no extension found from filename, try to determine from content type
    if (empty($extension) && !empty($contentType)) {
        $mimeToExt = [
            'model/gltf+json' => 'gltf',
            'model/gltf-binary' => 'glb',
            'application/octet-stream' => 'glb', // Common for binary files
        ];
        
        $extension = $mimeToExt[$contentType] ?? '';
    }
    
    return [
        'filename' => $filename ?? '',
        'extension' => $extension,
        'contentType' => $contentType
    ];
}

function getGoogleDriveFileContent($url) {
    // Extract file ID from URL
    $fileId = '';
    if (preg_match('/\/file\/d\/([^\/]+)/', $url, $matches)) {
        $fileId = $matches[1];
    } else if (preg_match('/id=([^&]+)/', $url, $matches)) {
        $fileId = $matches[1];
    } else {
        throw new Exception("Invalid Google Drive URL format");
    }
    
    // Get file info first
    $fileInfo = getGoogleDriveFileInfo($fileId);

    // Create download URL
    $downloadUrl = "https://drive.google.com/uc?export=download&id=" . $fileId;

/*
    
    // Initialize cURL
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $downloadUrl);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_COOKIEJAR, 'cookie.txt');
    curl_setopt($ch, CURLOPT_COOKIEFILE, 'cookie.txt');
    
    $response = curl_exec($ch);
    
    // Handle large file warning
    if (strpos($response, 'virus scan warning') !== false) {
        preg_match('/"downloadUrl":"([^"]+)"/', $response, $matches);
        if (isset($matches[1])) {
            $confirmUrl = str_replace('\\u003d', '=', str_replace('\\u0026', '&', $matches[1]));
            curl_setopt($ch, CURLOPT_URL, $confirmUrl);
            $response = curl_exec($ch);
        }
    }
    
    if (curl_errno($ch)) {
        throw new Exception('cURL error: ' . curl_error($ch));
    }
    
    curl_close($ch);
*/    
    return [
        'content' => "",
	'download' => $downloadUrl,
        'fileInfo' => $fileInfo
    ];
}

$countname = "./pp2/tmp.count";
$num = file_get_contents($countname);
$num2 = intval($num) + 1;
$str = sprintf('%05d',$num2);
file_put_contents($countname,$str);


$gltf = $_POST["gltffile"];
$gltf3 = $gltf;
$ext = "";
if (strpos($gltf,'drive.google.com') !== false) {
   $arr = getGoogleDriveFileContent($gltf);
   $fileinfo = $arr['fileInfo'];
   $filename = $fileinfo['filename'];
   $ext = $fileinfo['extension'];
   $gltf = $arr['download'];
}

$gltf = str_replace("&amp;","&",$gltf);

$gltf2 = explode('?',$gltf)[0];
if($ext=="") {
$ext = substr($gltf2, -4);
}

if (strlen($ext)<4||(substr($ext,-3)!="zip"&&substr($ext,-3)!="glb"&&substr($ext,-4)!="gltf")) {
  echo "<pre>";
  echo "ERROR, SOMETHING WRONG WITH THE URL YOU ENTERED(should be .zip,.glb or .gltf)\n";
  echo "URL WAS:$gltf3\n";
  echo "GLTF URL WAS: $gltf\n";
  echo "FILENAME WAS: $filename\n";
  echo "EXTENSION WAS: $ext</pre>";
  echo "FILEINFO WAS:";
  print_r($fileinfo,TRUE);
  echo "\n";
  $logstr = $_POST["gltffile"] . " " . $transparent . " " . $zoom . " " . $rotate . " " . $pan . " " . $shadow . " " . $anim . " " . $bigscreen . " " . $sketchfab . "-> FAIL\n";
$fp = fopen("./pp2/tmp.log","a+");
fwrite($fp, $logstr);
fclose($fp);

} else {

$is_zip = "nope";

if ($ext == ".zip")
{
  $is_zip = "yes";
}

$pos = strrpos($gltf2, '/');
$gltf_filename = substr($gltf2, $pos+1);

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
else {
   $file = "TF I1=ev.mainloop_api.gltf_loadKK2(" . $gltf . ");\n";
   }
if ($is_zip=="yes" && $sketchfab!="sketchfab") {
   $file = "ML I8=ev.mainloop_api.load_zip(ev," . $gltf . ");\n";   
} else {
if ($shadow=="shadow") {
   if ($large!="large") {
      $file .= "P I20=ev.polygon_api.gltf_load(ev,I1,0,0);\n";
      } else {
      $file .= "P I20=ev.mainloop_api.gltf_mesh_all_p(ev,I1);\n";
      }
   $file .= "MT I210=ev.materials_api.gltf_material(ev,I1,0,1,3.5,1.0,-400,400,400);\n";
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
  $file .= "MT I211=ev.materials_api.gltf_material(ev,I1,0,1,3.5,1.0,-400,400,400);\n";
  $file .= "MT I221=ev.materials_api.gltf_anim_material2(ev,I1,0,230,I211,c,0);\n";
  $file .= "ML I25=ev.materials_api.bind(I201,I221);\n";
  } else {

  $file .= "ML I25=ev.mainloop_api.gltf_mesh_all_anim(ev,I1,1,3.5,1.0,0,c,-400,400,400,0.0,ff000000);\n";
 }

} else {
   $file .= "ML I25=ev.mainloop_api.gltf_mesh_all(ev,I1,1,3.5,1.0,0,-400,400,400,0.0,ff000000);\n";
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
  $file .= "MT I21=ev.materials_api.gltf_material(ev,I1,0,1,3.5,1.0,-400,400,400);\n";
  $file .= "MT I22=ev.materials_api.gltf_anim_material2(ev,I1,0,230,I21,c,0);\n";
  $file .= "ML I23=ev.materials_api.bind(I20,I22);\n";
  $file .= "ML I2=ev.mainloop_api.send_key_at_time(I23,0.0,99);\n";
 } else {
  $file .= "ML I20=ev.mainloop_api.gltf_mesh_all_anim(ev,I1,1.0,3.5,1.0,0,c,-400,400,400,0.0,ff000000);\n";
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
  echo "URL WAS:$gltf3\n";
  echo "GLTF URL WAS: $gltf\n";
  echo "FILENAME WAS: $filename\n";
  echo "EXTENSION WAS: $ext</pre>";
  echo "FILEINFO WAS:";
  print_r($fileinfo,TRUE);
  echo "\n";

$logstr = "$file3" . "\n" . $_POST["gltffile"] . " " . $transparent . " " . $zoom . " " . $rotate . " " . $pan . " " . $shadow . " " . $anim . " " . $bigscreen . " " . $sketchfab . "-> FAIL\n";
$fp = fopen("./pp2/tmp.log","a+");
fwrite($fp, $logstr);
fclose($fp);

}
}