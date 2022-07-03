<?php

require_once("user.php");

$return = $_GET["return"];

page_header_plain(7);
page_title("login","");
echo "<div class=\"emscripten\">";
?>
<form action="mesh_login_result.php" method="post">
EMail address:<br>
<input type="text" name="email"><br>
Password:<br>
<input type="password" name="passwd"><br>
<br>
<input id="return" type="hidden" name="return">
<input type="submit">
</form>
<script>
  var e = document.getElementById("return");
  e.value = "<?php echo $return ?>";
</script>
<?php
echo "</div>";
page_footer();