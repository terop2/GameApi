<?php

?>
<html>
<body>
<div id="app">
<ul>
<template v-for="msg in message">
<div class="highlight">{{ msg }}</div>
</template>
</ul>
</div>

<script src="https://meshpage.org/vue.js"></script>
<script>
fetch("https://meshpage.org/functions.txt").then((r) => { 
  return r.text();
}).then((t) => {
var app = new Vue({
  el: '#app',
  data: { message: t.match(/"[^"]*"|\w+/g) }
})

})
</script>
<style>
.highlight {
  font-size: 25px;
  color: white;
  background-color: black;
  width: 200;
  matgin: 10px 10px 10px 10px;
  border-width: 5px;
  border-style: solid;
  border-color: black;
  text-overflow: ellipsis;
}
:hover.highlight {
  background-color: brown;
  border-width: 5px;
  border-style: outset;
  border-color: brown;
}
body {
  background-color: black;
}
</style>
</body>
</html>