<?php

require_once("user.php");

if ($_POST["email"]=="terop@kotiposti.net" && password_verify($_POST["passwd"],'$2y$10$W2vqEMu5a6cY7ogiq72Efek.PiR82TtiU9JiFSPhNLEWMqdw8PEdS'))
{
  set_user("terop_priv");
  header("Location: mesh_all.php");
}
else
{
page_header();
page_title("login result","");
echo "INCORRECT PASSWORD!";
}