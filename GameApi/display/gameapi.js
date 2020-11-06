
var pretag = document.getElementById("gameapi_script");
var data = pretag.innerHTML;
var lines = data.split("\n");
var lines2 = "";
for(var i = 0;i<lines.length;i++) {
    lines2 = lines2.concat(lines[i].trim()+"@");
}
var data4 = lines2.replace(/\t/g,"");
var canv = document.getElementById("canvas");
var hom = document.getElementById("homepage");
var hom2 = hom.innerHTML;
var Module = {
    canvas : canv,
    arguments : [ "--size", "800", "600", "--code", data4, "--homepage", hom2],
    print : (function() { return function(text) { console.log(text); } })(),
};
