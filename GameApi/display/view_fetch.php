<?php
ini_set("post_max_size", "120M");
ini_set("upload_max_filesize", "100M");
ini_set("memory_limit", "1024M");

include("backend.php");

function unhash($data)
{
   return ((intval($data)) ^ 0x26522663) /10001;
}

$id = $_GET["id"];

$user="terop";

if ($id>0)
{
  $state = load_form_state($user, $id);
  $contentsarray = "";
  $filenamearray = "";

  for($i=0;$i<500;$i++) {
  		       $contentsarray = $contentsarray . load_form_contentsarray($user, strval($id) . "_" . strval($i));
  		       $filenamearray = $filenamearray . load_form_filenamearray($user, strval($id) . "_" . strval($i));
  }
  $gfilename = load_form_gfilename($user,$id);
  $gpath = load_form_gpath($user, $id);

  $array = array( "id" => $id,
  	   	  "state" => $state,
		  "filenamearray" => $filenamearray,
		  "gfilename" => $gfilename,
		  "gpath" => $gpath,
    	   	  "contentsarray" => $contentsarray);

  header("Content-Type: application/json");
  echo json_encode($array);
}
else
{
  $array = array( "iid" => $iid,
  	   	  "id" => $id,
  	   	  "state" => "",
  	   	  "contentsarray" => "",
		  "filenamearray" => "",
		  "gfilename" => "",
		  "gpath" => "");

  header("Content-Type: application/json");
  echo json_encode($array);

}