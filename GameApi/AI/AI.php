<?php

?>

<html lang="en">
<head>
<meta charset="UTF-8">
<title>AI for creating 3d scenes</title>

<style>
div#box {
  margin: 0;
    height: 100vh;
    display: flex;
    flex-direction: column;
    justify-content: center;   /* center horizontally */
    align-items: center;       /* center vertically */
}
#prompt {
  width: 500px;
   border-radius: 10px;
  border-width: 3px;
  border-style: solid;
  border-color: black;
  font-size: 150%;
}
#promptimage {
   border-radius: 30px;
   border-width: 3px;
   border-style: solid;
}
#label {
  font-size: 150%;

}
#labeldiv {
   margin: 8px;
}
#horiz {
   display: flex;
   flex-direction: row;
}
#buttondiv {
  margin: 8px;
}
</style>

</head>
<body>
<div id="box">
<div id="title">
<h1>meshpage.org/AI.php</h1>
</div>
<div id="subtitle">
<h3>"fake it until you can make it"</h3>
</div>
<div style="height: 30px">
</div>
<div id="image">
<img id="promptimage" src="promptimage.png?t=<?= time() ?>" width="600px"/>
</div>
<div style="height: 30px">
</div>
<div id="promptdiv">
<form action="/submit" method="get">
<div class="horizcontainer" id="horiz">
<div class="labeldiv" id="labeldiv">
<label class="label" for="prompt"><b>Prompt:</b></label>
</div>
<input type="text" id="prompt" name="prompt" required>
<div class="buttondiv" id="buttondiv">
<button class="submit" type="submit"><b>Submit</b></button>
</div>
</div>
</form>
</div>
</div>
<script>
var counter = 0;
function change_image(a)
{
   var arr = ["build_1.png", "build_2.png", "build_3.png"];

   var p = document.getElementById("promptimage");
   p.src = arr[a];
   counter++;
   if (counter>2) { counter=0; }
   window.setTimeout(function() { change_image(counter); },8000);
}
change_image(counter);

</script>


</body>
</html>