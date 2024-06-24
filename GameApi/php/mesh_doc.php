<?php

$num1 = $_GET["menu"];
$num2 = $_GET["submenu"];
$flag = $_GET["select"];

$cmd1 = "./dump_docs " . $num1;
$cmd2 = "./dump_docs " . $num1 . " " . $num2;
$cmd3 = "./dump_docs " . $num1 . " " . $num2 . " " . $flag;

$res1 = shell_exec($cmd1);
$res2 = shell_exec($cmd2);
$res3 = shell_exec($cmd3);

echo "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
echo "$res1";
echo "<hr>";
echo "$res2";
echo "<hr><pre>";
echo "$res3";
echo "</pre>";