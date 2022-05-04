<?php

function mappings()
{
   $data = file_get_contents("mapping.txt");
   $arr = explode("\n", $data);
   $res = array();
   foreach( $arr as $item ) {
     $arr2 = explode(" ", $item);
     $res[] = $arr2;
   }
  return $res;
}

function source_data( $data )
{
   $array = explode( "\n", $data );

   $linenum = 1;
   $map = mappings();
   $arr = array();
   foreach( $array as $line )
   {
     $comment = "";
     foreach($map as $item) {
       if ($item[0] != "") {
         $pos = strpos( $line, $item[0] );
         if ($pos != FALSE) {
            $comment = $item[1];
         }
       }
     }
   $line0 = explode(" ", $line);
   $type = $line0[0];
   $rest = $line0[1];
   $line1 = explode("=", $rest);
   $label = $line1[0];
   $rest2 = $line1[1];
   $arr[] = array($linenum, $comment, $type, $label, $rest2);

     $linenum++;
  } 
  return $arr;
}

function source_table( $data, $id )
{
   $arr = source_data( $data );
   echo "<div class=\"mytable\">";
   $num = count($arr);

    //$linenum = $item[0];
    //$comment = htmlspecialchars($item[1]);
    //$type = htmlspecialchars($item[2]);
    //$label = htmlspecialchars($item[3]);
    //$rest = htmlspecialchars($item[4]);

    echo "<div class=\"myrow\">";
          echo "<div class=\"mycolumn0\">";
	  foreach($arr as $item2) {
	     $linenum = htmlspecialchars($item2[0]);
	     echo "$linenum<br>"; 
	  }
	  echo '</div>';
     	  echo "<div class=\"mycolumn1\">";
	  foreach($arr as $item2) {
	     $comment = htmlspecialchars($item2[1]);
	     echo "$comment<br>"; 
	  }
	  echo "</div>";

       echo "<div class=\"mycolumn2\">";
       foreach ($arr as $item2 ) {
           $type = htmlspecialchars($item2[2]);
    	   $label = htmlspecialchars($item2[3]);
    	   $rest = htmlspecialchars($item2[4]);
	   $rt = explode("(",$item2[4]);
	   $rr = explode(".",$rt[0]);
	   $rr0 = htmlspecialchars($rr[0]);
	   $rr1 = htmlspecialchars($rr[1]);
	   $rr2 = htmlspecialchars($rr[2]);
	   $rt0 = htmlspecialchars($rt[0]);
	   $rt1 = htmlspecialchars($rt[1]);
          echo "$type $label=$rr0.$rr1.<a href=\"mesh_api.php?id=$id#$rr2\">$rr2</a>($rt1<br>";
       }
       echo "</div>";
     echo "</div>";
   echo "</div>";
}