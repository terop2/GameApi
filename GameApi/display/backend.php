<?php

function save_form_state($user, $id, $state)
{
  $filename = "viewdata/user_" . $user . "/state_" . $id . ".txt";
  file_put_contents($filename, $state);
}
function load_form_state($user, $id)
{
  $filename = "viewdata/user_" . $user . "/state_" . $id . ".txt";
  return file_get_contents($filename);
}
function save_form_contentsarray($user, $id, $contentsarray)
{
  $filename = "viewdata/user_" . $user . "/contents_" . $id . ".txt";
  file_put_contents($filename, $contentsarray);
}
function load_form_contentsarray($user, $id)
{
  $filename = "viewdata/user_" . $user . "/contents_" . $id . ".txt";
  return file_get_contents($filename);
}

function save_form_filenamearray($user, $id, $filenamearray)
{
  $filename = "viewdata/user_" . $user . "/files_" . $id . ".txt";
  file_put_contents($filename, $filenamearray);
}
function load_form_filenamearray($user, $id)
{
  $filename = "viewdata/user_" . $user . "/files_" . $id . ".txt";
  return file_get_contents($filename);
}
function save_form_gfilename($user, $id, $gfilename)
{
  $filename = "viewdata/user_" . $user . "/gfilename_" . $id . ".txt";
  file_put_contents($filename, $gfilename);
}
function load_form_gfilename($user, $id)
{
  $filename = "viewdata/user_" . $user . "/gfilename_" . $id . ".txt";
  return file_get_contents($filename);
}
function load_num()
{
  $filename = "viewdata/num.txt";
  $file = file_get_contents($filename);
  return intval( $file );
}
function save_num( $val )
{
  $filename = "viewdata/num.txt";
  $data = strval($val);
  file_put_contents($filename,$data);
}