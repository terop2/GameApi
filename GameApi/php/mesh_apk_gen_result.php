<?php

$num = $_GET["num"];
if (is_numeric($num))
{
  system("cp /home/terop/meshpage.org/user_data/user_terop/text$num.txt /home/terop/cvs/GameApi/GameApi/cmdline4/script.txt");
  system("(cd /home/terop/GameApi/GameApi/cmdline4;./ammake2.sh)";
  $file = file_get_contents("/home/terop/GameApi/GameApi/cmdline4/gameapi_cmdline.apk");
  header("Content-Type: vnd.android.package-archive");
  header("Content-Disposition: attachment; filename=\"android$num.apk\"");
  echo "$file";
} else {
  echo "ERROR!";
  }