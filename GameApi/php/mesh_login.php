<?php

require_once("user.php");

page_header(7);
page_title("login","");
echo "<div class=\"emscripten\">";
?>
<form action="mesh_login_result.php" method="post">
EMail address:<br>
<input type="text" name="email"><br>
Password:<br>
<input type="password" name="passwd"><br>
<br>
<input type="submit">
</form>
<?php
echo "</div>";
page_footer();