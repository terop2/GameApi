<?php
include "backend.php";
//print_r($_POST);

$num = $_POST["num"];
if ($num==-1) {
$id = load_num();
$id = $id + 1;
save_num($id);
echo "$id";
} else {
echo "<pre>";
$id = load_num();
$id2 = strval($id) . "_" . strval($num);
$state = $_POST["state"];
$contentsarray = $_POST["contents_array"];
$filenamearray = $_POST["filename_array"];
$gfilename = $_POST["g_filename"];
$gpath = $_POST["g_path"];

$user = "terop";
if ($num=="0")
   save_form_state($user, $id, $state);
save_form_contentsarray($user, $id2, $contentsarray);
save_form_filenamearray($user, $id2, $filenamearray);
save_form_gpath($user, $id2, $gpath);
if ($num=="0")
   save_form_gfilename($user, $id, $gfilename);
echo "<a href='https://meshpage.org/view.php?id=" . $id . "'>https://meshpage.org/view.php?id=" . $id . "</a>";
echo "</pre>";
}