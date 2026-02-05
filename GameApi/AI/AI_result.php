<?php
$keys = $_GET["prompt"] ?? '';
$keywords = explode(' ', $keys);
$keywords_json = json_encode($keywords);
?>

<html>
<head>
<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.8/dist/css/bootstrap.min.css" rel="stylesheet">
</head>
<body>
<form id="iframeform" method="POST" action="AI_display.php" target="myIframe">
<input type="hidden" name="keywords" value="<?= htmlspecialchars($keywords_json, ENT_QUOTES, 'UTF-8') ?>"/>
</form>
<div class="d-flex justify-content-center align-items-center vh-100">
<iframe id="myIframe" name="myIframe" style="border-style: none;" scrolling="no" width="800" height="600"></iframe>
</div>
<script>
  document.getElementById("iframeform").submit();
</script>
<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.8/dist/js/bootstrap.bundle.min.js" integrity="sha384-FKyoEForCGlyvwx9Hj09JcYn3nv7wiPVlz7YYwJrWVcXK/BmnVDxM+D2scQbITxI" crossorigin="anonymous"></script>


</body>
</html>