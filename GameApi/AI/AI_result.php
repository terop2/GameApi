<?php
$keys = $_GET["prompt"];
$keywords = explode(' ', $keys);
$keywords_json = json_encode($keywords);
?>

<html>
<body>
<form id="iframeform" method="POST" action="AI_display.php" target="myIframe">
<input type="hidden" name="keywords" value="<?= htmlspecialchars($keywords_json, ENT_QUOTES, 'UTF-8') ?>"/>
</form>
<iframe id="myIframe" name="myIframe" style="border-style: none;" scrolling="no" width="800" height="600"></iframe>

<script>
  document.getElementById("iframeform").submit();
</script>


</body>
</html>