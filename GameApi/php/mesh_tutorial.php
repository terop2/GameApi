<?php

require_once("user.php");

page_header(4);
page_title("documents","");
echo "<div class=\"emscripten\">";

list_start("Available documents:");
list_item("http://tpgames.org/Linux_compile.txt", "Linux compilation");
list_item("http://tpgames.org/Feature_list.txt", "Builder Feature list");
list_item("http://tpgames.org/Opengl_chain.txt", "Mesh structure");
list_item("http://tpgames.org/Math_concepts.txt", "Math concepts");
list_item("http://tpgames.org/Printer.txt", "3d printer instructions");
list_item("http://tpgames.org/Skills_to_learn.txt", "Skills to learn");
list_end();
echo "<div>";

page_footer();