<?php
  //header("Cross-Origin-Opener-Policy: same-origin");
  $id = $_GET["id"];
  $filename = "./user_data/temp/tmp" . $id . ".txt";
  $script = file_get_contents($filename);
  $homepage = $_GET["homepage"];
  $new_script = str_replace("@","\n",$script);
  $date = $_GET["date"];
  ?>
<html>
  <head>
  </head>
  <body>
    <!-- you need to copy the following files to transfer 3d models
         to new web server:
         https://meshpage.org/gameapi_display.zip

	 unzip the package to some new directory in your web host

	 Then change the homepage url from inside pre tag below

	 Then copy-paste a builder codegen script to the gameapi_script 
	 pre tag below, last line needs to have type RUN.
      -->
    <pre id="homepage" style="display:none">
<?php echo $homepage ?>
    </pre>
    <pre id="gameapi_script" style="display:none">
<?php echo $new_script ?>
    </pre>
    <pre id="gameapi_modificationdate" style="display:none">
<?php echo $date ?>
    </pre>
    <div class="center">
    <canvas id="canvas" style="margin:0; padding:0; width: 820px; height: 620px;"></canvas>
    </div>
    <style>
    .center {
       display: flex;
       align-items: center;
       justify-content: center;
    }
    </style>
    <script src="gameapi.js?<?php echo $date ?>"></script>
      
  </body>
</html>
