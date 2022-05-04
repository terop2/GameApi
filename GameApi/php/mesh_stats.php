<?php
require_once("user.php");
page_header();
page_title("stats","");
echo "<div class=\"emscripten\">";
?>
<ul>
<li>main page: <?php echo visit_counter_get("mainpage") ?>
<li>createmesh: <?php echo visit_counter_get("createmesh") ?>
<li>mesh page: <?php echo visit_counter_get("mesh") ?>
<li>tool page: <?php echo visit_counter_get("tool") ?>
</ul>
</div>
<?php
page_footer();