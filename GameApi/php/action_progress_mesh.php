<?php
require_once("user.php");
$data = $_POST["meshdata"];
file_put_contents("meshdata.txt", $data);
$num = read_num("terop");
$num = $num + 1;
increase_num("terop", $num);

?>
<html>
<progress id="progressor" value="22" max="100"></progress>
<div id="percentage">0%</div>
<form action="create_mesh.php" method="post">
<button type="submit" >Back</button>
<input type="hidden" name="meshdata" value="<?php echo $data ?>">
</form>
<form action="user_page.php" method="post">
<div id="continue" style="display:none">
<button type="submit">Continue..</button>
</div>
</form>
<div id="results">Results:<br></div>
<script>
var es;
function start_task() {
   es = new EventSource("action_new_mesh.php");
   es.addEventListener('message', function(e) {
      var result = JSON.parse(e.data );
      addLog( result.message );
      var pBar = document.getElementById('progressor');
      pBar.value = result.progress;
      var perc = document.getElementById('percentage');
      perc.innerHTML = result.progress + "%";
      if (result.progress == 100) { 
           es.close();
           var k = document.getElementById('continue');
           k.style.display="inline";
      }
      });
   es.addEventListener('error', function(e) {
      addLog('Error occured');
      es.close();
   });
}
function stop_task() {
   es.close();
   addLog("Interrupted");
}
function addLog(message) {
  var r = document.getElementById("results");
  r.innerHTML += message + '<br>';
  r.scrollTop = r.scrollHeight;
}
start_task();
</script>
<?php
