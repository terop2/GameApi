<?php

require_once("user.php");

visit_counter_inc("createmesh");

function create_form()
{
$meshdata = "";
if (array_key_exists("meshdata",$_POST)) {
  $meshdata = $_POST["meshdata"];
}
$user="terop";
$num = read_num( $user );
$num = $num + 1;

$arr = array("username" => $user,
             "index" => $num,
	     "width" => 800,
	     "height" => 600,
	     "id" => "800x600");
$id = create_id( $arr );
?>
<form action="mesh_preview.php?id=<?php echo $id ?>" enctype="multipart/form-data" method="post">
<b>Give your homepage url</b>:<br>
<input type="text" name="homepageurl" id="homepage" size="60"/>
<div class="tooltip">gdpr
<span class="tooltiptext">
<div class="info" style="border-style:solid; font-size:13px;">
<ul>
<li>Field: Homepage URL
<li>Purpose: Copyright checking
<li>Legal basis: Filter out copyright infringing material before publication
<li>Controller address: terop@kotiposti.net
<li>Data collection officer: terop@kotiposti.net
<li>Recipients: meshpage.org
<li>Period of storage: Infinite
<li>Criteria of storage period: Info is needed while the content is available in meshpage.org web site
<li>There's right to request(check controller address):
<ul>
<li>rectification
<li>erasure
<li>withdrawing consent
</ul>
<li>There's right to lodge a complaint
<li>The data is required for copyright checking purposes
<li>Consiquences of not providing the data: some content items requiring network access are not visible
<li>Logic: Data is used for domain checking, i.e. homepage address is compared against content item's web domain. Check faq for more info: <a href="mesh_faq.php">faq</a>
</ul>
</div>
</span>
</div>
<p>
<b>Add .png file screenshot:</b><br>
<input type="file" name="pngfilename" id="filetoupload">
<p>
<b>Choose a label for your animation:</b><br>
<input type="text" name="label" id="displayedlabel">
<p>
<b>Please copy-paste the code from builder application CodeGen output to the textbox below, needs to be type RUN:</b><br>
<textarea name="meshdata" rows="30" cols="80"><?php echo $meshdata ?></textarea>
<input type="submit" value="Create Mesh" style="width:300; height:50; font-size:30;"/>
</form>

<?php
}
page_header(1);
page_title("creating new mesh","");
echo "<div class=\"emscripten\">";
create_form();
echo "<div>";
page_footer();