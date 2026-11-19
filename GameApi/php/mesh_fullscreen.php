<?php
   $id = $_GET["id"];
   $filename3 = "user_data/user_terop/fullscreen" . $id . ".txt";
   $data = file_get_contents($filename3);
   echo "$data";
   