<?php
include "backend.php";
echo "<pre>";
//print_r($_POST);

$num = $_POST["num"];
if ($num==-1) {
$id = load_num();
$id = $id + 1;
save_num($id);
} else {
$id = load_num();
$id2 = strval($id) . "_" . strval($num);
$state = $_POST["state"];
$contentsarray = $_POST["contents_array"];
$filenamearray = $_POST["filename_array"];
$gfilename = $_POST["g_filename"];

$user = "terop";
if ($num=="0")
   save_form_state($user, $id, $state);
save_form_contentsarray($user, $id2, $contentsarray);
save_form_filenamearray($user, $id2, $filenamearray);
if ($num=="0")
   save_form_gfilename($user, $id, $gfilename);
echo "<a href='https://meshpage.org/view.php?id=" . $id . "'>https://meshpage.org/view.php?id=" . $id . "</a>";
echo "</pre>";
}