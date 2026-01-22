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
  border-width: 3px;
  border-style: solid;
  border-color: black;
  font-size: 150%;
}
</style>

</head>
<body>
<div id="box">
<div id="image">
<img src="promptimage.png?t=<?= time() ?>" width="600px"/>
</div>
<div style="height: 30px">
</div>
<div id="promptdiv">
<form action="/submit" method="get">
<label for="prompt">Prompt:</label>
<input type="text" id="prompt" name="prompt" required>
<button type="submit">Submit</button>
</form>
</div>
</div>
</body>
</html>