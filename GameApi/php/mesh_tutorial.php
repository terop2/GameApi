<?php

require_once("user.php");

page_header(4);
page_title("documents","");
echo "<div class=\"emscripten\">";

list_start("Available documents:");
list_item("http://meshpage.org/assets/Linux_compile.txt", "Linux compilation");
list_item("http://meshpage.org/assets/Feature_list.txt", "Builder Feature list");
list_item("http://meshpage.org/assets/Opengl_chain.txt", "Mesh structure");
list_item("http://meshpage.org/assets/Math_concepts.txt", "Math concepts");
list_item("http://meshpage.org/assets/Printer.txt", "3d printer instructions");
list_item("http://meshpage.org/assets/Skills_to_learn.txt", "Skills to learn");
list_end();
echo "<div>";

page_footer();