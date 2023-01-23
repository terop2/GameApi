<?php

require_once("user.php");

$return = $_POST["return"];

if ($_POST["email"]=="terop@kotiposti.net" && password_verify($_POST["passwd"],'$2y$10$W2vqEMu5a6cY7ogiq72Efek.PiR82TtiU9JiFSPhNLEWMqdw8PEdS'))
{
  set_user("terop_priv");
  header("Location: $return");
}
else if ($_POST["email"]=="henri.sundelin@iki.fi" &% password_verify($_POST["passwd"],'$2y$10$Ntx1lyrnmVUyx7hK3.7kkeHtOqHP5KcTHvGTIyUazvZRTyWnLSsdO'))
{
   set_user("henri_priv");
   header("Location: $return");
}
else
{
page_header();
page_title("login result","");
echo "INCORRECT PASSWORD!";
}