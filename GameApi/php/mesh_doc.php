<?php

$num1 = $_GET["menu"];
$num2 = $_GET["submenu"];
$flag = $_GET["select"];

$cmd = "./dump_docs " . $num1 . " " . $num2 . " " . $flag;

$res = shell_exec($cmd);

echo "$res";