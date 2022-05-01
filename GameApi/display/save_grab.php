<?php

$id = $_POST["id"];
$num = $_POSR["num"];

$filename = "user_data/user_terop/grab" . $id . "_" . $num . ".png";
$file = file_get_contents("php://input");
file_put_contents($filename, $file);
