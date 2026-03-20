<?php

$num1 = $_GET["menu"];
$num2 = $_GET["submenu"];
$flag = $_GET["select"];

if (!ctype_digit($num1) || !ctype_digit($num2) || !($flag === "select"))
{
 http_response_code(400);
 echo "Invalid input";
 exit;
}


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
echo "<hr>";
echo "Warning: copying code from these files should not be done to your own 3d engine. The code has potential to break your engine structure, if you do it, because the design uses tricks that you are not able to understand, even if it looks simple. It really does something very strict constraints on surrounding code, which is enforced by the compiler, low level and the type system. Steam and opengl has marked important pieces as dangerous. And it extends to large area of the code. The problem is completely invisible also/difficult to find the real reason.";