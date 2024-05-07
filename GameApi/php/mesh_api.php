<?php

require("user.php");
function load_classes($filename)
{
   $cont = file_get_contents($filename);
   $arr = explode("£", $cont);
   $res = array();
   $i = 0;
   foreach($arr as $item)
   {
   $k = explode("@",$item);
   if (count($k)>=2)
      $res[] = array("class"=>$k[0],"contents"=>$k[1]);
   $i = $i + 1;
   }
   return $res;
}
function load_funcs($filename)
{
  $cont = file_get_contents($filename);
  $arr = explode("£", $cont);
  $res = array();
   $i = 0;
  foreach($arr as $item)
  {
   $arr2 = explode("@",$item);
   if (count($arr2)>=3)
   $res[] = array("sig"=>$arr2[0],"link"=>$arr2[1],"contents"=>$arr2[2]);
   $i = $i + 1;
  }
  return $res;
}

function convert($d)
{
$arr= array( "MainLoopApi","mainloop_api",
  "PointApi","point_api",
  "VectorApi","vector_api",
  "MatrixApi","matrix_api",
  "SpriteApi","sprite_api",
  "GridApi","grid_api",
  "BitmapApi","bitmap_api",
  "PolygonApi","polygon_api",
  "BoolBitmapApi","bool_bitmap_api",
  "FloatBitmapApi","float_bitmap_api",
  "ContinuousBitmapApi","cont_bitmap_api",
  "FontApi","font_api",
  "AnimApi","anim_api",
  "EventApi","event_api",
  "FunctionApi","function_api",
  "VolumeApi","volume_api",
  "FloatVolumeApi","float_volume_api",
  "ColorVolumeApi","color_volume_api",
  "DistanceFloatVolumeApi","dist_api",
  "VectorVolumeApi","vector_volume_api",
  "ShaderApi","shader_api",
  "StateChangeApi","state_change_api",
  "TextureApi","texture_api",
  "SeparateApi","separate_api",
  "WaveformApi","waveform_api",
  "ColorApi","color_api",
  "LinesApi","lines_api",
  "PlaneApi","plane_api",
  "PointsApi","points_api",
  "VoxelApi","voxel_api",
  "FrameBufferApi","fbo_api",
  "SampleCollectionApi","sample_api",
  "TrackerApi","tracker_api",
  "ShaderModuleApi","sh_api",
  "WModApi","mod_api",
  "PhysicsApi","physics_api",
  "TriStripApi","ts_api",
  "CutterApi","cutter_api",
  "BooleanOps","bool_api",
  "CollisionPlane","collision_api",
  "MovementNode","move_api",
  "ImplicitApi","implicit_api",
  "PickingApi","picking_api",
  "TreeApi","tree_api",
  "MaterialsApi","materials_api",
  "UberShaderApi","uber_api",
  "CurveApi","curve_api",
  "MatricesApi","matrices_api",
  "Skeletal","skeletal_api",
  "PolygonArrayApi","polygon_arr_api",
  "PolygonDistanceField","polygon_dist_api",
  "BlockerApi","blocker_api",
  "VertexAnimApi","vertex_anim_api",
  "NewPlaneApi","newplane_api",
  "SurfaceApi","surface_api");

  $i = 0;
  foreach($arr as $item) 
  {
    if ($item == $d) return $arr[$i-1];
    $i = $i + 1;
  }
  return $d;
}
function before_a ($this_b, $inthat_b)
    {
        return substr($inthat_b, 0, strpos($inthat_b, $this_b));
    }
function pretty($str)
{
	return htmlspecialchars($str);
}

function print_data($classes, $funcs, $apiname, $funcname)
{
	//var_dump($apiname);
        $flag = false;
	foreach($funcs as $item) {
           $k = strpos( $item["sig"], $apiname );
	   $p = strpos( $item["sig"], "::" . $funcname . "(" );
	   if ($p > 0 && $k > 0) {
              $link = $item["link"];
	      $linkarr = explode(":",$link);
	      if ($link=="") { $flag=true; }
	      foreach($linkarr as $link) {
	          foreach($classes as $cls) {
	             if ($cls["class"]==$link) {
		        $val = $cls["contents"];
		        $val = pretty($val);
		        print "<a name=\"$funcname\">$val</a>";
                        $flag = true;
		        }
		     }
                   }
                   if ($flag==true) {
		      $val2 = $item["contents"];
		      $val2 = pretty($val2);
		      print "<a name=\"$funcname\">$val2</a>";
		      $flag=false;
		   
		 }
	      }
           }
	}

$data = $_GET["id"];
//var_dump($data);
$arr = decode_id( $data );
//var_dump($arr);
$user = $arr["username"];
$num = read_num( $user );
$arr2 = array("username" => $user,
              "index" => intval($arr["index"]),
	      "num" => intval($arr["index"])+1,
	      "width" => 800,
	      "height" => 600);
$code = user_code( $arr2, 0 );

//var_dump($code);

echo "<html>";
echo "<head>";

small_page_header_head( $arr2 );
small_page_header_body( $arr2, 3);
page_title("source code","");
//var_dump("test");
$codearr = explode("\n",$code . "\n");
//var_dump($codearr);
//var_dump("test2");

$classes = load_classes("https://meshpage.org/assets/data.txt");
$funcs = load_funcs("https://meshpage.org/assets/data2.txt");

foreach($codearr as $item) {
  $arr = explode(".",$item);
  $api = $arr[1];
  $funcname = before_a("(",$arr[2]);

  echo "<pre>";
  print_data($classes, $funcs, convert($api),$funcname);
  echo "</pre>";
}
$code = pretty($code);
echo "<pre>";
echo "int main()\n{\n";
echo "$code\n";
echo "}\n";
echo "</pre>";

echo "</html>";
echo "</head>";
