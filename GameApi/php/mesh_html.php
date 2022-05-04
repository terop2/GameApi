<?php

require_once("user.php");

$data = $_GET["id"];
$arr = decode_id( $data );
$user = $arr["username"];
$num = read_num( $user );
$arr2 = array("username" => $user,
              "index" => intval($arr["index"]),
	      "num" => intval($arr["index"])+1,
	      "width" => 800,
	      "height" => 600);

function embeds($data, $sx, $sy) {
$ssx = $sx+30;
$ssy = $sy+50;
$html = "<embed width=\"$ssx\" height=\"$ssy\" src=\"https://www.meshpage.org/mesh.php?id=" . $data . "&noheaders=1&width=$sx&height=$sy\"/>";
$html = htmlspecialchars($html);

$skx = $sx+50;
$sky = $sy+80;


$html2 = "<embed width=\"$skx\" height=\"$sky\" src=\"https://www.meshpage.org/mesh_clickable.php?id=" . $data . "&noheaders=1&width=$sx&height=$sy\"/>";
$html2 = htmlspecialchars($html2);
return array($html,$html2);
}
$htmlarr1 = embeds($data,400,300);
$htmlarr2 = embeds($data,800,600);
?>
<html>
<head>
<?php small_page_header_head( $arr2 ); ?>
</head>
<body>
<?php small_page_header_body( $arr2,4 ); ?>
<?php page_title("Html embed",""); ?>
<form>
Standard html embed (400x300):
<input type="text" size="100" width="800" height="400" value="<?php echo $htmlarr1[0] ?>">
<p>
Clickable html embed (400x300):
<input type="text" size="100" width="800" height="400" value="<?php echo $htmlarr1[1] ?>">
<p>
<br>
<p>
Standard html embed (800x600):
<input type="text" size="100" width="800" height="400" value="<?php echo $htmlarr2[0] ?>">
<p>
Clickable html embed (800x600):
<input type="text" size="100" width="800" height="400" value="<?php echo $htmlarr2[1] ?>">

</form>
</body>
</html>