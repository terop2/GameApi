<?php

require_once("user.php");

unset_user("terop");
unset_user("terop_priv");
header("Location: mesh_all.php");