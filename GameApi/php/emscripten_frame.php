<?php

function encode( $data )
{
   $i = 0;
   $len = strlen( $data );
   $str = "";
   for($i = 0;$i < $len;$i++)
   {
      $ch = $data[$i];
      if ($ch=="\r") { } else
      if ($ch=="\n") { $str .= "@\\\n"; } else
      $str .= $ch;
   }
   return $str;
}

function frame( $arr, $width, $height, $data, $i, $homepage, $noheaders, $seamless, $transparent )
{
$lowmem = is_lowmem( $arr );
$version = js_version( $arr );
$nothreads = js_no_threads( );
$mobile = js_mobile();
if ($nothreads == "yes") {
   $nothfile = "_nothreads";
   echo "(nothreads)";
} else {
   $nothfile = "";
}
if ($version=="" && $seamless!=1 ) 
{ 
//echo "Browser: OK (webassembly/binaryen)"; 
}
if ($version=="_asmjs" && $seamless!=1) 
{ 
//echo "Browser: SLOW (webassembly missing => asmjs fallback used)";
}
$lowfile = "";
if ($mobile == "yes") {
   //echo " (mobilelowmem)\n"; 
   $lowfile = "_lowmem";
}
if ($lowfile == "") {
if ($lowmem == 0) { 
   //echo " (normal)\n"; 
   $lowfile = ""; 
   } else { 
   //echo " (lowmem)\n"; 
   $lowfile = "_lowmem"; 
   }
}
$filename = "engine" . $version . $lowfile . $nothfile . ".js?" . js_id();
$homepage = trim(preg_replace('/\s+/', ' ', $homepage));
$user="terop";
$index = $arr["index"];
$imgname = "user_data/user_" . $user . "/screenshot" . $index . ".png";
$ua = $_SERVER["HTTP_USER_AGENT"];

?>
    <div class="spinner" id='spinner<?php echo $i ?>' style="display:none"></div>
    <div class="emscripten" id="status<?php echo $i ?>" style="width:300; height:80; font-size:30; display:none;">Downloading...</div>
 <input type="checkbox" id="resize<?php echo $i ?>" hidden=1>
  <input type="checkbox" id="pointerLock<?php echo $i ?>" hidden=1 checked>
      <progress value="0" max="100" id="progress<?php echo $i ?>" style="display:none;"></progress>
<?php
if ($noheaders==NULL && $seamless == NULL) {
//echo "<br>";
echo "<input style=\"width:300; height:50; font-size:30; display:none;\" id=\"fullscreen\" type=\"button\" value=\"Fullscreen\" onclick=\"Module.requestFullscreen(document.getElementById('pointerLock$i').checked,document.getElementById('resize$i').checked)\">";

//echo "<input style=\"width:100; height:50; font-size:30; display:none;\" id=\"vrdisplay\" type=\"button\" value=\"VR\" onclick=\"Module.setVR(document.getElementById('pointerLock$i').checked,document.getElementById('resize$i').checked)\">";

}
?>

<div class="emscripten_border" id="border">
<canvas tabindex="0" style="width:<?php echo $width ?>px; height:<?php echo $height ?>px; display:none;" width=<?php echo $width ?> height=<?php echo $height ?> class="emscripten" id="canvas" oncontextmenu="event.preventDefault()"></canvas>
<canvas id="rect" width="<?php echo $width ?>" height="<?php echo $height ?>"></canvas>

</div>
    <textarea id="output<?php echo $i ?>" rows="30" cols="80" style="display:none" ></textarea>
    <div class="emscripten">
    </div>
    <pre id='pretag<?php echo $i ?>' style="display:none">
    <?php echo $data ?>
    </pre>
    <img style="display:none" id="scr" width="<?php echo $width?>" height="<?php echo $height ?>" draggable="false" src="<?php echo $imgname ?>"/>
    <div itemscope itemtype="http://schema.org/CreativeWork">
    <script type='text/javascript'>
      //var cc = document.getElementById("canvas");
      //var ctx2 = cc.getContext("webgl");
      //ctx2.disable = function(x0) { }
      //ctx2.enable = function(x0) { }
      var c = document.getElementById("rect");
      var ctx=c.getContext("2d");
      var xx = <?php echo $width ?>;
      var yy = <?php echo $height ?>;
<?php
  if ($seamless == NULL) {
  if ($width < 700) {
?>
      ctx.rect(0,0,xx,yy);
      ctx.fill();
<?php } ?>
      ctx.fillStyle="white";
      var num = yy*100/600;
      ctx.font=num.toString() + "px Arial";
      ctx.fillText("qtamoVR",xx*100/800,yy*300/600);
      var num2 = yy*30/600;
      ctx.font=num2.toString() + "px Arial";
      ctx.fillText("Loading...", xx*160/800,yy*330/600);
<?php
   }
?>      
<?php
if ($noheaders==NULL && $seamless == NULL) {
?>
      var b = document.getElementById("fullscreen");
<?php
}
?>
      //var bf = document.getElementById("vrdisplay");

      var statusElement = document.getElementById('status<?php echo $i ?>');
      var progressElement = document.getElementById('progress<?php echo $i ?>');
      var spinnerElement = document.getElementById('spinner<?php echo $i ?>');

      var Module = {
      	__register_pthread_ptr : function() { },
        arguments: ["--size", "<?php echo $width ?>", "<?php echo $height ?>", "--code", "<?php echo encode($data) ?>", "--screenshot", "scr.ppm", "3000.0", "--homepage", "<?php echo $homepage ?>", "--seamless", "<?php echo $seamless ?>", "--transparent", "<?php echo $transparent ?>","--platform","<?php echo $ua ?>"],
        preRun: [],
        postRun: [],
	print: (function() {

          //var element = document.getElementById('output<?php echo $i ?>');
          //if (element) element.value = ''; // clear browser cache
          return function(text) {
            if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
            // These replacements are necessary if you render to raw HTML
            //text = text.replace(/&/g, "&amp;");
            //text = text.replace(/</g, "&lt;");
            //text = text.replace(/>/g, "&gt;");
            //text = text.replace('\n', '<br>', 'g');
            console.log(text);
            //if (element) {
            //  element.value += text + "\n";
            //  element.scrollTop = element.scrollHeight; // focus on bottom
            //}
          };
        })(),
        printErr: function(text) {
          if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
          if (0) { // XXX disabled for safety typeof dump == 'function') {
            dump(text + '\n'); // fast, straight to the real console
          } else {
            console.error(text);
          }
        },
        canvas: (function() {
          var canvas = document.getElementById("canvas");
          // As a default initial behavior, pop up an alert when webgl context is lost. To make your
          // application robust, you may want to override this behavior before shipping!
          // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
          canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
	  

          return canvas;
        })(),
	vr_displays: function() {
          return navigator.getVRDisplays();
	},
	onAnimFrame: function(t) {
           console.log("onAnimFrame");
	},
	setVR: function(lock, resize) {
	   if (navigator.getVRDisplays) {
	   var disp = navigator.getVRDisplays().then(function(disp) {
	   console.log("display searching!");
	   if (disp.length>0) {
	   console.log("display found!");
	   var d = disp[0];
	   console.log("display: " + d.displayName);
	   var canvas = document.getElementById("canvas");
	   console.log("canvas found!");
	   var input = document.getElementById("vrdisplay");
	   input.addEventListener("click", function() {
	   console.log("click!");
	   d.requestPresent([{ source: canvas }]).then(function(displays) { console.log("WOw"); 
	   d.requestAnimationFrame(Module.onAnimFrame);
	   d.submitFrame();
});
	   },false);
	   console.log("request_present done");
	   //Module.requestFullscreen(lock,resize);
	   }
	   });
	   }
	   //activate_vr = Module.cwrap('activate_vr', 'number', ['number']);
	   //activate_vr(1);
	},
		
        setStatus: function(text) {
	   console.log(text);
          if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
          if (text === Module.setStatus.text) return;
          var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
          var now = Date.now();
          //if (m && now - Date.now() < 30) return; // if this is a progress update, skip it if too soon
          if (m) {
	    console.log(m[2]);
	    console.log(m[4]);
            //text = m[1];
            progressElement.value = parseInt(m[2])*100;
            progressElement.max = parseInt(m[4])*100;
            //progressElement.hidden = false;
            //spinnerElement.hidden = false;
          } else {
            //progressElement.value = null;
            //progressElement.max = null;
            //progressElement.hidden = true;
            //if (!text )spinnerElement.style.display = 'none';
          }
          statusElement.innerHTML = text;
	  if (text=="") {
	     c.style.display= 'none';
	     var c2 = document.getElementById("canvas");
	     c2.style.display= 'block';
<?php
if ($noheaders==NULL && $seamless == NULL) {
?>
	     //b.style.display= 'inline-block';
	     //bf.style.display= 'inline-block';
<?php
 }
?>
          } else
	  if (text.search("Running")!=-1)
	  {
<?php
  if ($seamless == NULL) {
?>

	        ctx.fillStyle="black";
<?php
  if ($width < 700) {
?>
        	ctx.rect(0,0,xx*800/800,yy*600/600);
<?php } else { ?>
        	ctx.rect(20,20,xx*800/800-20,yy*600/600-20);
<?php } ?>
      		ctx.fill();
	     ctx.fillStyle="white";
	           var num = yy*100/600;
      		   ctx.font=num.toString() + "px Arial";
	     ctx.fillText("qtamoVR",xx*100/800,yy*300/600);
      var num2 = yy*30/600;
      ctx.font=num2.toString() + "px Arial";

	     ctx.fillText(text, xx*160/800,yy*330/600);
<?php
}
?>
	  } else
	  if (text.search("Exception")!=-1)
	  {	
	        ctx.fillStyle="black";
<?php
  if ($width < 700) {
?>
        	ctx.rect(0,0,xx*800/800,yy*600/600);
<?php } else { ?>
        	ctx.rect(20,20,xx*800/800-20,yy*600/600-20);
<?php } ?>

      		ctx.fill();
	     //ctx.fillStyle="white";
	     //      var num = yy*100/600;
      	//	   ctx.font=num.toString() + "px Arial";
	     //ctx.fillText("Error",xx*100/800,yy*300/600);
	     var img = document.getElementById("scr");
	     ctx.drawImage(img,0,0,<?php echo $width ?>,<?php echo $height ?>);
      var num2 = yy*30/600;
      ctx.font=num2.toString() + "px Arial";
      <?php if ($width < 700) { ?>
	     //ctx.fillText("try chrome://flags, webassembly threads enabled", xx*120/800,yy*330/600);
      <?php
       }
      if ($width >= 700) {
      ?>
      //ctx.fillText("please try about:config to set shared_memory=true", xx*120/800,yy*330/600);
       <?php } ?>
	  }
        },
        totalDependencies: 6176847,
        monitorRunDependencies: function(left) {
	  console.log(left);
          this.totalDependencies = Math.max(this.totalDependencies, left);
	  //Module.setStatus("Preparing... (2/4)"); 
          Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
        }
      };
<?php
  if ($seamless == 1) {
?>
      var canvas = document.getElementById("canvas");
      Module.preinitializedWebGLContext = canvas.getContext('webgl', {premultipliedAlpha:false, preserveDrawingBuffer: false, alpha: true});
<?php
 }
?>
      //var canvas = document.getElementById("canvas");
      //Module.preinitializedWebGLContext = canvas.getContext('2d', {premultipliedAlpha:false, preserveDrawingBuffer: false, alpha: true});



      Module.setStatus('Downloading...');
      function onDisplayPointerRestricted() {
      	console.log('requestpointerlock');
        var canvas = document.getElementById("canvas");
        canvas.requestPointerLock();
      }
      function onDisplayPointerUnrestricted() {
      	console.log('exitpointerlock');

                document.exitPointerLock();
      }
      
/*
      function onVRRequestPresent(canvas) {
        display.requestPresent([{source: canvas}]).then(function() {
           onDisplayPointerUnrestricted();
           presentingCanvas = canvas;
        }, function(err) {
	   Module.printErr("requestPresent failed: " + err);
        }
      }
*/
      //window.addEventListener('vrdisplaypointerrestricted', onDisplayPointerRestricted);
      //window.addEventListener('vrdisplaypointerunrestricted', onDisplayPointerUnrestricted);
      //window.addEventListener('vrdisplaypresentchange', function(e) {
     // 	console.log('Display ' + e.display.displayId + ' presentation has changed. Reason given: ' + e.reason + '.');
      //});

      //window.addEventListener('vrdisplaypresentchange', function() {
      //  if (display.isPresenting) {
      //     presentingCanvas = null;
      //   }
      //}, false);
      window.onerror = function(event) {
        // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
        Module.setStatus('Exception thrown, see JavaScript console');
        //spinnerElement.style.display = 'none';
        Module.setStatus = function(text) {
          if (text) Module.printErr('[post-exception status] ' + text);    

        };
      };
    </script>
<script async type="text/javascript" src="<?php echo $filename ?>"></script>

    </div>
<?php
}
