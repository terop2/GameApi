<?php
ini_set("post_max_size", "120M");
ini_set("upload_max_filesize", "100M");
ini_set("memory_limit", "1024M");

include("backend.php");
header("Cross-Origin-Opener-Policy: same-origin");
$date = filemtime("web_page_highmem.js");

function unhash($data)
{
   return ((intval($data)) ^ 0x26522663) /10001;
}

$iid = $_GET["id"];
$id=unhash($_GET["id"]);
//echo "ID=$id";
$user = "terop";
if ($id>0)
{
  $state = load_form_state($user, $id);
  $contentsarray = "";
  $filenamearray = "";
  for($i=0;$i<500;$i++) {
  		       $contentsarray = $contentsarray . load_form_contentsarray($user, strval($id) . "_" . strval($i));
  		       $filenamearray = $filenamearray . load_form_filenamearray($user, strval($id) . "_" . strval($i));
  }
  $gfilename = load_form_gfilename($user, $id);
  $gpath = load_form_gpath($user,$id);
}
?>
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, shrink-to-fit=no"/>
</head>
<body>
<?php

echo "<pre id='formstate2' style='display:none'>";
echo "$state";
echo "</pre>";

echo "<pre id='formcontentsarray2' style='display:none'>";
echo "$contentsarray";
echo "</pre>";

echo "<pre id='formfilenamearray2' style='display:none'>";
echo "$filenamearray";
echo "</pre>";

echo "<pre id='formgfilename2' style='display:none'>";
echo "$gfilename";
echo "</pre>";

echo "<pre id='formgpath2' style='display:none'>";
echo "$gpath";
echo "</pre>";



?>

<script src="vue.js"></script>
<div id="app">
<appdragdroparea v-on:dragdrop="dragdrop2($event)">

<apptitle><a href="view.php">The 3d model viewer</a></apptitle>
<br>
<div style="display:flex" id="dp">
<div id="div2" style="display:none"></div>
<!--/div-->
<div style="margin-left: 30px; float:left; display:block;">
<appcanvas></appcanvas>
</div>
<div style="margin-left: 30px; float:left; display:block;">
<p>
<appmodel v-bind:is_example="state.appmodel_is_examples"
	  v-bind:is_link="state.appmodel_is_link"
	  v-bind:is_selected="state.appmodel_is_selected"
	  v-bind:is_notselected="state.appmodel_is_notselected"
	  v-bind:is_loading="state.appmodel_is_loading"
	  v-bind:is_twoline="state.appmodel_is_twoline"
	  v-bind:model_info="state.model_info"
	  v-on:examples_click="change_appmodel(2)"
	  v-on:link_click="change_appmodel(4)"
	  v-on:change_model="change_model()"
	  v-on:change_choose="change_model()"
	  v-on:change_choose_url="change_model_url()"
	  v-on:root_handle_drop="change_model3($event)"
	  v-bind:filename="state.filename"
	  v-bind:filename1="state.filename1"
	  v-bind:filename2="state.filename2"
></appmodel>
<appbackground v-on:change_model="change_model()"></appbackground>
<appmaterial v-on:change_model="change_model()"
	     v-on:change_category="change_category()"
	     v-bind:is_metal="state.is_metal"
	     v-bind:is_plastic="state.is_plastic"
	     v-bind:is_textured="state.is_textured"
	     v-bind:filter_material_metal="filter_material_metal()"
	     v-bind:filter_material_plastic="filter_material_plastic()"
	     v-bind:filter_material_textured="filter_material_textured()"
></appmaterial>
<appborder v-on:change_model="change_model()"></appborder>
<appnormals v-on:change_model="change_model()"></appnormals>
<appsubmitbutton></appsubmitbutton>
<br>
</div>
</div>
<div style="height:10px"></div>
<div style="height:10px"></div>
</appdragdroparea>
</div> <!-- vue ends here -->
</body>
</html>

<script>
var store = {
   state : {
      empty: true,
      appmodel_is_examples: "false",
      appmodel_is_selected: "false",
      appmodel_is_notselected: "false",
      appmodel_is_loading: "true",
      appmodel_is_twoline: "0",
      model_info: "(prepare to load..)",
      filename: "(no file)",
      filename1: "",
      filename2: "",
      url: "",
      is_metal: "true",
      is_plastic: "false",
      is_textured: "false",
      model_db: [],
      background_db: [],
      material_db: [],
      border_db: [],
      file_ap: function(type,filename) { }, // should return promise
      }
      };

Vue.component('apptitle', {
   data: function() {
       return {
          }
        },
	template: `<div class="block"><div class='lab' id='lab'><h1 class="customfont"><slot></slot></h1></div></div>`
 });

Vue.component('appsubmitbutton', {
    data: function() {
       return { }
       },
       template: `<div class="block blockitem height8 border customfont">
           <!-- form id="submitcontents" action="submit_contents.php" method="POST">
	   <input name="state" id="formstate" type="hidden" value="@"/>
	   <input name="contents_array" id="formcontentsarray" type="hidden"/>
	   <input name="filename_array" id="formfilenamearray" type="hidden"/>
	   <input name="g_filename" id="formgfilename" type="hidden"/>
	   <input name="g_path" id="formgpath" type="hidden"/>
	   <input name="num" id="formnum" type="hidden"/>
	   </form -->
	   <button type="button" onclick="formsubmit()">Submit</button>
	   <div id="progressbar"></div>
	   </div>`
	   });

Vue.component('appdragdroparea', {
   data: function() {
       return {
          }
	},
	template: '<div id="div1" v-on:drop="$emit(`dragdrop`,$event)" ondragover="allowDrop(event)"><slot></slot></div>'
	});

Vue.component('appnormals', {
   data: function() {
      return { } },
   template: `<div>
       <div class="block blockitem height8 border customfont">
       Mode:
        <select name="normals" id="normals-select" v-on:change="$emit('change_model')">
        <option value="-1">Default</option>
	<option value="0">Edged normals</option>
        <option value="1">Smooth normals</option>
        <option value="2">File normals</option>
	<option value="3">Wireframe</option>
	<option value="4">Wireframe+hidden line removal</option>
	<option value="5">Wireframe+Edged normals</option>
	<option value="6">Wireframe+smooth normals</option>
        </select>
       </div></div>`
 });    
       
Vue.component('appmaterial', {
   props: ['is_metal', 'is_plastic', 'is_textured', 'filter_material_metal', 'filter_material_plastic', 'filter_material_textured'],
   data: function() {
      return {
      }
      },
      template: `<div>
	<div class="block blockitem height16 border customfont"><!--Material<br>
	<div class="horizspace customfont">
		<select name="material" id="material-select" v-on:change="$emit('change_model')">
		 <option value="-1">Default</option>
		 <option value="0">EdgedPhong</option>
		 <option value="1">EdgedMetal</option>
  		 <option value="2">EdgedGold</option>
  		 <option value="6">Wood1</option>
  		 <option value="7">Wood2</option>
  		 <option value="8">Wood3</option>
  		 <option value="9">Shiny Wood1</option>
  		 <option value="10">Shiny Wood2</option>
  		 <option value="11">Shiny Wood3</option>
		 </select></div>-->
		 Category<br>
		 <div class="horizspace">
		 <select name="category" id="category-select" v-on:change="$emit('change_category')">
		 <option value="0">Metal</option>
		 <option value="1">Plastic</option>
		 <option value="2">Textured</option>
		 </div>
		 <div v-if="is_metal=='true'">
		 Type<br>
		 <div class="horizspace customfont">
		 <select name="type" id="metal-type-select" v-on:change="$emit('change_model')">
		 <option value="-1">Default</option>
		 <template v-for="metal in filter_material_metal">
		 <option v-bind:value="parse_material_count(metal,'Metal')">{{ parse_material_name(metal,'Metal') }}</option>
		 </template>
		 </select>
		 </div>
		 </div>
		 <div v-if="is_plastic=='true'">
		 Type<br>
		 <div class="horizspace customfont">
		 <select name="type" id="plastic-type-select" v-on:change="$emit('change_model')">
		 <option value="-1">Default</option>
		 <template v-for="plastic in filter_material_plastic">
		 <option v-bind:value="parse_material_count(plastic,'Plastic')">{{ parse_material_name(plastic,'Plastic') }}</option>
		 </template>
		 </select>
		 </div>
		 </div>

		 <div v-if="is_textured=='true'">
		 Type<br>
		 <div class="horizspace customfont">
		 <select name="type" id="textured-type-select" v-on:change="$emit('change_model')">
		 <option value="-1">Default</option>
		 <template v-for="tex in filter_material_textured">
		 <option v-bind:value="parse_material_count(tex,'Textured')">{{ parse_material_name(tex,'Textured') }}</option>
		 </template>
		 </select></div>
		 </div>
		 <!--Surface<br>
		 <div class="horizspace customfont">
		 <select name="surface" id="surface-select">
		 <option value="0">Smooth</option>
		 <option value="1">Brushed</option>
		 <option value="2">Hammered</option>
		 </select>
		 </div>-->
		 `
});

Vue.component('appbackground', {
  data: function() {
    return {
    }
    },
    template: `
    <div class="block blockitem height8 border customfont">
    Background<br>
    <div class="horizspace customfont">
    <select name="background" id="background-select" v-on:change="$emit('change_model')">
      <!--option value="-1">Black</option-->
    <template v-for="bg in store.state.background_db">
    <option v-bind:value="parse_bg_count(bg)">{{ parse_bg_name(bg) }}</option>
    </template>
    </select></div></div>
    `
});

Vue.component('appborder', {
  data: function() {
     return {
     }
     },
     template: `<div class="block blockitem height8 border customfont">
     Border<br><div class="horizspace customfont"><select name="border" id="border-select" v-on:change="$emit('change_model')">
  <!--option value="-1">None</option -->
  <template v-for="brd in store.state.border_db">
  <option v-bind:value="parse_border_count(brd)">{{ parse_border_name(brd) }}</option>
  </template>
  </select></div></div>
     `
});

Vue.component('appmodel_choose', {
  data: function() {
     return {
     }
     },
     template: `<div class="border block blockitem height12 customfont">
     Model<br><select name="model" id="model-select" v-on:change="$emit('change_model')">
 <option value="-1">(use drag&drop area)</option>
 <template v-for="model in store.state.model_db">
 <option v-bind:value="parse_model_count(model)">{{ parse_model_name(model) }}</option>
 </template>
</select></div>
     `
     });
Vue.component('appmodel_link', {
  data: function() {
     return {
     }
     },
     template: `<div class="border block blockitem height12 customfont">
     Link<br><input type="url" id="model_url" name="url" v-on:change="$emit('change_model')"/>
   <button type="button" v-on:click="$emit('change_model')" style="margin-right:0; margin-left: auto; display: block; width: 80px; height: 30px">Open</button>

</div>
     `
     });

Vue.component('appmodel_loading', {
  data: function() {
     return { } },
     template: `<div class="border block blockitem height12 customfont">
      Loading 3d engine...
      </div>`
      });

Vue.component('appmodel_notselected', {
   data: function() {
     return { } },
   template: `<div class="border block blockitem height12 customfont">
   <small>Please Drag & Drop any 3D model to this page. You can also try our <a href="javascript:;" v-on:click="$emit('examples_click')">examples</a><!--or <a href="javascript:;" v-on:click="$emit('link_click')">link</a-->.</small>
   <br><br><small>.STL, .OBJ, .GLB, .ZIP file types supported. See help for materials.</small>
   <button type="button" onclick="clickselectfile()" style="margin-right:0; margin-left: auto; display: block; width: 80px; height: 30px">Open</button>
   <input id="selectfile" type="file" multiple v-on:change="$emit('handle_drop','selectfile')" style="display:none"/>
   </div>`
   });

Vue.component('appmodel_selected', {
  props: ['filename', 'filename1', 'filename2', 'model_info', 'is_twoline'],
  data: function() {
    return { } },
    template: `<div class="border block blockitem height12 customfont">
      <!--appprogress></appprogress-->
      <appfilenameinfo v-bind:is_twoline="is_twoline" v-bind:filename="filename" v-bind:filename1="filename1" v-bind:filename2="filename2"></appfilenameinfo>
      <appmodelinfo v-bind:model_info="model_info"></appmodelinfo>
      <button type="button" onclick="clickselectfile2()" style="margin-right:0; margin-left: auto; display: block; width: 80px; height: 30px">Open</button>
   <input id="selectfile2" type="file" multiple v-on:change="$emit('handle_drop','selectfile2')" style="display: none;"/>
      </div>`
      });

Vue.component('appmodel', {
  props: ['is_example', 'is_link', 'is_selected', 'is_notselected', 'is_loading', 'filename', 'filename1', 'filename2', 'model_info', 'is_twoline'],
  data: function() {
    return { } },
    template: `<div class="block blockitem">
       <div v-if="is_loading=='true'">
       <appmodel_loading></appmodel_loading>
       </div>
       <div v-if="is_example=='true'">
       <appmodel_choose v-on:change_model="$emit('change_choose')"></appmodel_choose>
       </div>
       <div v-if="is_link=='true'">
       <appmodel_link v-on:change_model="$emit('change_choose_url')"></appmodel_link>
       </div>
       <div v-if="is_selected=='true'">
       <appmodel_selected v-bind:model_info="model_info" v-bind:filename="filename" v-bind:filename1="filename1" v-bind:filename2="filename2" v-bind:is_twoline="is_twoline" v-on:handle_drop="$emit('root_handle_drop',$event)"></appmodel_selected>
       </div>
       <div v-if="is_notselected=='true'">
       <appmodel_notselected v-on:examples_click="$emit('examples_click')"
       			     v-on:link_click="$emit('link_click')"
			     v-on:handle_drop="$emit('root_handle_drop',$event)"
       ></appmodel_notselected>
       </div>
       </div>`
       });


Vue.component('appcanvas', {
  data: function() {
     return {
     }
     },
     template: `<div class="block">
<canvas class="canvas" id="canvas" oncontextmenu="event.preventDefault()"></canvas>
     </div>
     `
});

Vue.component('appinfo', {
  data: function() {
     return {
     }
     },
     template: `<div class="block blockitem customfont">
<h3>Supported file formats</h3>
.gltf, .glb, .stl, .obj, .mtl, .ds, dirs, .zip
<h3>Supported texture formats</h3>
.jpg, .png</div>
     `});


Vue.component('appprogress', {
  data: function() {
     return {
     }
     },
     template: `
     <div class="info block blockitem" id="label customfont">
     drag&drop here.
     </div>
     `
});

Vue.component('appfilenameinfo', {
  props: ['filename', 'filename1', 'filename2', 'is_twoline'],
  data: function() {
    return {
     }
     },
     template: `<div>
       <div v-if="is_twoline=='2'">
          <div class="info block blockitem height4 customfont">
	  <b>{{ filename1 }}<br>{{ filename2 }}</b>
	  </div>
       </div>
       <div v-if="is_twoline=='1'">
          <div class="info block blockitem middle height4 customfont">
	  <b>{{ filename }}</b>
	  </div>
       </div>
       <div v-if="is_twoline=='0'">
          <div class="info block blockitem large height4 customfont">
          <b>{{ filename }}</b>
          </div>
       </div>
    </div>`
     });

Vue.component('appmodelinfo', {
  props: ['model_info'],
  data: function() {
     return {
     }
     },
     template: `<div class="info block blockitem horizspace customfont">
     {{ model_info }}
     </div>`
     });

var repeat_prev = 0;

var app = new Vue({
   el: '#app',
   data: {
      state: store.state
      },
   computed: {


   },
   methods: {
parse_material_type : function(mat)
{
   var arr = mat.split(" ");
   return arr[1];
},

filter_material : function(arr,key)
{
   var arr = store.state.material_db;
  //console.log(arr);
  var s = arr.length;
  var res = [];
  for(var i=0;i<s;i++) {
     var type = parse_material_type(arr[i]);
     if (type==key) { res.push(arr[i]); }
  }
  return res;
},
   
   filter_material_metal : function() {
   var arr = store.state.material_db;
   return this.filter_material(arr,'Metal');
},
   filter_material_plastic: function() {
   var arr = store.state.material_db;
   return this.filter_material(arr,'Plastic');
},
   filter_material_textured: function() {
   var arr = store.state.material_db;
   return this.filter_material(arr,'Textured');
},


      dragdrop2: function(event) {
         //console.log("DRAGDROP");
	 //console.log(event);
        if (repeat_prev==0) {
        repeat_prev=1;
          this.change_appmodel(1);
         drop(event);
	    repeat_prev = 0;
	    }
      },
      change_appmodel: function(val) {
         this.state.appmodel_is_notselected = "false";
	 this.state.appmodel_is_selected = "false";
	 this.state.appmodel_is_examples = "false";
	 this.state.appmodel_is_loading = "false";
	 this.state.appmodel_is_link = "false";
         if (val==0) this.state.appmodel_is_notselected = "true";
	 if (val==1) this.state.appmodel_is_selected = "true";
	 if (val==2) this.state.appmodel_is_examples = "true";
	 if (val==3) this.state.appmodel_is_loading = "true";
	 if (val==4) this.state.appmodel_is_link = "true";
      },
      change_category: function() {
        var elem = document.getElementById("category-select");
	var val = elem.value;
	this.state.is_metal="false";
	this.state.is_plastic="false";
	this.state.is_textured="false";
	//console.log("change_category:" + val);
	if (val==0) { this.state.is_metal="true"; }
	if (val==1) { this.state.is_plastic="true"; }
	if (val==2) { this.state.is_textured="true"; }
      },

      change_model: function() {
        if (repeat_prev==0) {
        repeat_prev=1;
        //console.log("CHANGE_MODEL");
	  if (typeof drop2 == 'function')
            drop2(this.state);
	    }
          this.change_appmodel(1);
	    repeat_prev = 0;
      },
      change_model_url: function() {
         var inputtag = document.getElementById("model_url");
	 var url = inputtag.value;
	 this.state.url = url;
	 //console.log(url);
	 this.change_model();
      },
      change_model3: function(selectfile) {
        if (repeat_prev==0) {
        repeat_prev=1;
        //console.log("CHANGE_MODEL3");
	//console.log(selectfile);
	if (typeof drop3 =='function')
           drop3(this.state, selectfile);
	}
        this.change_appmodel(1);
        repeat_prev = 0;
      }
   }
      
   });
   
var mat_db = "material_db.txt";
var bor_db = "border_db.txt";
var bck_db = "background_db.txt";
var mdl_db = "model_db.txt";

fetch(mat_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.material_db = str.split("\n");
   store.state.material_db.pop();
//console.log(store.state.material_db);
   });
});
fetch(bor_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.border_db = str.split("\n");
   store.state.border_db.pop();
   //console.log(store.state.border_db);
   });
});
fetch(bck_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.background_db = str.split("\n");
   store.state.background_db.pop();
  
   //console.log(store.state.background_db);
   });
});
fetch(mdl_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.model_db = str.split("\n");
   store.state.model_db.pop();
   //console.log(store.state.model_db);
   });
});


function strfy(arr)
{
  const array = Array.from(arr);
  let str = "";
  for (let i=0;i<array.length; i++) {
    str+=String.fromCharCode(parseInt(array[i]));
    }
   return str;
}


</script>
<style>
.lab { width:70%; height: 30px; text-align:center; }
.canvas { border-width:0px;border: 5px solid black; border-radius: 10px; background-color: #000000; margin:0; padding:0; width: 820px; height: 620px; }
.block { display: block;   }
.info { margin: auto; text-align: left; padding: 3px; }
.prettysmall { font-size: 25px; }
.middle { font-size: 30px; }
.large { font-size: 40px; }
.border { border: 1px solid black; padding: 15px 15px 0px; 35px; margin-top: -1px; }
.blockitem { width: 250px;}
.height4 { height: 40px; }
.height8 { height: 80px; }
.height12 { height: 120px; }
.height16 { height: 160px; }
.horizspace { margin-left: 1em; }
@font-face { font-family: "custom"; src: url("cafe.ttf"); }
.customfont { font-family: custom; }
</style>

<script>

function find_main_item(arr)
{
   var s = arr.length;
   for(var i=0;i<s;i++) {
      if (arr[i].substr(-4)==".zip") return i;
      if (arr[i].substr(-4)==".glb") return i;
      if (arr[i].substr(-4)==".stl") return i;
      if (arr[i].substr(-5)==".gltf") return i;
      if (arr[i].substr(-3)==".ds") return i;
      if (arr[i].substr(-4)==".obj") return i;
      //if (arr[i].name.substr(-4)==".ply") return i;
   }
   set_label("ERROR: main item not found");
   //console.log("ERROR: main item not found");
   return -1;
}
function default_script()
{
   var str="";
   str+="P I1=ev.polygon_api.p_empty();@";
   str+="ML I2=ev.polygon_api.render_vertex_array_ml2(ev,I1);@";
   str+="RUN I3=ev.blocker_api.game_window2(ev,I2,false,false,0.0,100000.0);@";
   return str;
}
function convert_enter_to_at(str)
{
  var s = str.length;
  var res = "";
  for(var i=0;i<s;i++) {
    if (str[i]=="\n") res +='@';
    else res+=str[i];
  }
  return res;
}
function get_base_dir(filename)
{
  var res = "";
  var s = filename.split("/");
  var ss = s.length;
  for(var i=0;i<ss-1;i++) {
    res+=s[i];
    if (i!=ss-2) res+="/";
  }
  if (res[0]=="/") return res.substr(1);
  return res;
}
function find_mtl_name(filenames)
{
   var s = filenames.length;
   for(var i=0;i<s;i++) {
      var filename = filenames[i];
      //console.log(filename);
      if (filename.substr(-4)==".mtl") return filenames[i];
   }
   return "";
}
function get_model_value()
{
  var elem = document.getElementById("model-select");
  if (elem) return parseInt(elem.value);
  return -1;
}
function get_model_html(str)
{
  return "<option value=\"" + get_model_count(str) + "\">{{ parse_model_name(model) }}</option>";
}
function parse_model_count(str)
{
  var arr = str.split(" ");
  return arr[0];
}
function parse_model_filename(name)
{
   var arr = name.split(' ');
   return arr[1];
}
function parse_model_name(name)
{
   var arr = name.split(' ');
   return arr[2];
}
function get_model(i)
{
   var model = "";
   if (i>=0 && i<store.state.model_db.length) {
      var name2 = store.state.model_db[i];
      var name = parse_model_filename(name2);
      model = "https://tpgames.org/" + name;
   }

   //if (i==0) model="https://tpgames.org/wooly_sheep.stl";
   //if (i==1) model="https://tpgames.org/BoomBox.glb";
   //if (i==2) model="https://tpgames.org/Duck.glb";
   //if (i==3) model="https://tpgames.org/Astronaut.glb";
   return model;
}
function get_normals_value()
{
  var elem = document.getElementById("normals-select");
  return parseInt(elem.value);
}
function get_border_value()
{
  var elem = document.getElementById("border-select");
  return parseInt(elem.value);
}
function parse_border_count(brd)
{
   var arr = brd.split(" ");
   return arr[0];
}
function parse_border_name(brd)
{
   var arr = brd.split(" ");
   return arr[1] + " " + arr[2];
}
function parse_border_color(brd)
{
   var arr = brd.split(" ");
   return arr[3];
}
function parse_border_width(brd)
{
   var arr = brd.split(" ");
   return arr[4];
}
function get_border_width(i)
{
  var width = "0";
  if (i>=0 && i<store.state.border_db.length) {
     var name2 = store.state.border_db[i];
     width = parse_border_width(name2);
  }
  return width;
}
function get_border(i,m,filename)
{
  var color = "000000";
  var width = "0";
  if (i>=0 && i<store.state.border_db.length) {
     var name2 = store.state.border_db[i];
     color = parse_border_color(name2);
     width = parse_border_width(name2);
  }
  var res = "";
  var variable = "I2";
  if (m==-1) variable="I1";
  
  if (width=="0") {
   res+="P I5021=ev.polygon_api.recalculate_normals(" + variable + ");\n";
   res+="P I5023=ev.polygon_api.color(I5021,0);\n"
   res+="ML I5022=ev.polygon_api.render_vertex_array_ml2(ev,I5023);\n"
  } else {

  res+= "P I205=ev.polygon_api.recalculate_normals(" + variable + ");\nP I206=ev.polygon_api.smooth_normals2(I205);\n"
  res+= "MT I504=ev.materials_api.phong(ev,I4,0.0,0.0,1.0,ffffccaa,fffff8ee,30.0);\n";
  res+= "MT I501=ev.materials_api.toon_border(ev,I504," + width + ",ff" + color + ");\n";
if (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf") {
  res+="ML I5022=ev.materials_api.bind(I206,I501);\n";
  } else {
    res+="ML I5022=ev.materials_api.bind(I206,I501);\n";
  }
}
  res+="ML I503=ev.mainloop_api.depthmask(I5022,false);\n";
  res+="ML I502=ev.mainloop_api.depthfunc(I503,3);\n";
  return res;
}
function parse_bg_count(bg)
{
   var arr = bg.split(" ");
   return arr[0];
}
function parse_bg_name(bg)
{
  var arr = bg.split(" ");
  return arr[1];
}
function parse_bg_colour(bg)
{
  var arr = bg.split(" ");
  return arr[2];
}
function get_background_value()
{
  var elem = document.getElementById("background-select");
  return parseInt(elem.value);
}
function get_background(i)
{
  var color = "000000";
  if (i>=0 && i<store.state.background_db.length) {
    var name2 = store.state.background_db[i];
    var name = parse_bg_colour(name2);
    color = name;
  }
  /*
  if (i==0) color="888888";
  if (i==1) color="ffffff";
  if (i==2) color="0088ff";
  if (i==3) console.log("ERROR: custom color not implemented");
  */
  var background="BM I41=ev.bitmap_api.newbitmap(100,100,ff" + color + ");\nBM I42=ev.bitmap_api.scale_bitmap_fullscreen(ev,I41);\nML I43=ev.sprite_api.vertex_array_render(ev,I42);\nML I44=ev.sprite_api.turn_to_2d(ev,I43,0.0,0.0,800.0,600.0);\n";
  return background;
}



function parse_material_count(mat)
{
   var arr = mat.split(" ");
   return arr[0];
}
function parse_material_name(mat)
{
   var arr = mat.split(" ");
   return arr[2];
}
function parse_material_type(mat)
{
   var arr = mat.split(" ");
   return arr[1];
}
function parse_material_url(mat)
{ // this only works with textured
   var arr = mat.split(" ");
   return arr[3];
}
function parse_material_color(mat)
{
   var arr = mat.split(" ");
   return arr[3];
}
function parse_material_roughness(mat)
{
   var arr = mat.split(" ");
   return arr[4];
}
function find_line_from_material_db(index)
{
  var s = store.state.material_db.length;
  for(var i=0;i<s;i++) {
     var elem2 = store.state.material_db[i];
     var cnt = parse_material_count(elem2);
     if (cnt==index) {
        //console.log(cnt);
	//console.log(index);
     	return store.state.material_db[i];
	}
  }
  return "";
}


function get_material_value()
{
  var cat_elem = document.getElementById("category-select");
  if (cat_elem) {
  var cat_val = parseInt(cat_elem.value);
  var id = "";
  if (cat_val==0) // metal
  {
     id = "metal-type-select";  
  } else if (cat_val==1) // plastic
  {
    id="plastic-type-select";
  }
  else if (cat_val==2) // Textured
  {
    id="textured-type-select";
  }

  var elem = document.getElementById(id);
  if (elem) return parseInt(elem.value);
  return 0;
  } else return 0;
}
function hex_color_to_number(text)
{

	if (text) {
        var hash = 0;
        var digit = 0;
	var digit2 = '0';
        for (var i = 0; i < text.length; i++)
        {
	    digit2 = text.charAt(i);
	    if (digit2=='0') digit=0;	
	    if (digit2=='1') digit=1;
	    if (digit2=='2') digit=2;
	    if (digit2=='3') digit=3;
	    if (digit2=='4') digit=4;
	    if (digit2=='5') digit=5;
	    if (digit2=='6') digit=6;
	    if (digit2=='7') digit=7;
	    if (digit2=='8') digit=8;
	    if (digit2=='9') digit=9;
	    if (digit2=='a'||digit2=='A') digit=10;	
	    if (digit2=='b'||digit2=='B') digit=11;
	    if (digit2=='c'||digit2=='C') digit=12;
	    if (digit2=='d'||digit2=='D') digit=13;
	    if (digit2=='e'||digit2=='E') digit=14;
	    if (digit2=='f'||digit2=='F') digit=15;
	    //console.log(digit);
            hash = digit + (hash << 4);
        }

        var c = (hash & 0x00FFFFFF);
        //c = c - 16777216;
        //console.log(text);
	//console.log(c.toString(16));
        return c;
	} return 0;

}
function get_metal_color(i)
{
  var line = find_line_from_material_db(i);
  var str = parse_material_color(line);
  return hex_color_to_number(str);
}

function get_metal_roughness(i)
{
  var line = find_line_from_material_db(i);
  var str = parse_material_roughness(line);
  return parseFloat(str);
}

function get_material(i,filename)
{
// these are all I1 => I2 :: P->P
var texcoords = "P I2=ev.polygon_api.texcoord_plane(I1,-20,20,-20,20);\n";
var normals = "P I2=ev.polygon_api.recalculate_normals(I1);\n";
var smoothnormals = "P I200=ev.polygon_api.recalculate_normals(I1);\nP I2=ev.polygon_api.smooth_normals2(I200);\n";
var objnormals = "P I2=ev.polygon_api.translate(I1,0.0,0.0,0.0);\n";
var texcoord_normals = "P I200=ev.polygon_api.texcoord_plane(I1,-20,20,-20,20);\nP I2=ev.polygon_api.recalculate_normals(I200);\n"
var texcoord_smoothnormals = "P I200=ev.polygon_api.texcoord_plane(I1,-20,20,-20,20);\nP I201=ev.polygon_api.recalculate_normals(I200);\nP I2=ev.polygon_api.smooth_normals2(I201);\n"
var texcoord_objnormals = "P I2=ev.polygon_api.texcoord_plane(I1,-20,20,-20,20);\n";


var normals_select = normals;
var texcoord_normals_select = texcoord_normals;
var normals_val = get_normals_value();
if (normals_val==-1) {
if (filename.substr(-4)==".glb"||filename.substr(-4)==".obj"||filename.substr(-4)==".zip") { normals_val=2; } else { normals_val=0; }
}
if (normals_val==0||normals_val==5) { normals_select = normals; texcoord_normals_select = texcoord_normals; }
if (normals_val==1||normals_val==6) { normals_select = smoothnormals; texcoord_normals_select = texcoord_smoothnormals; }
if (normals_val==2) { normals_select = objnormals; texcoord_normals_select = texcoord_objnormals; }

var phongmaterial = "MT I4=ev.materials_api.phong(ev,I3,-0.3,0.3,-1.0,ffff8800,ffffffff,5.0);\n";

if (i==-1) return ["",normals_select];
;
 var metal_color = get_metal_color(i);
 var r = (metal_color&0xff0000)>>16;
 var g = (metal_color&0xff00)>>8;
 var b = (metal_color&0xff);
 var rr = r/255.0;
 var gg = g/255.0;
 var bb = b/255.0;
 var rrs = rr.toString();
 var ggs = gg.toString();
 var bbs = bb.toString();

 var rough = get_metal_roughness(i);
 var roughstr = rough.toString();

 var metal = "MT I4=ev.materials_api.gltf_material3(ev," + roughstr + ",0.95," + rrs + "," + ggs + "," + bbs + ",1,1);\n";

 var plastic = "MT I4=ev.materials_api.gltf_material3(ev," + roughstr +",0.1," + rrs + "," + ggs + "," + bbs + ",1,1);\n";


var line = find_line_from_material_db(i);


if (parse_material_type(line)=='Metal') return [metal,normals_select];
if (parse_material_type(line)=='Plastic') return [plastic,normals_select];


var tex_url = parse_material_url(line);
var textured_material = "MT I4=ev.materials_api.gltf_material_from_file(ev," + tex_url + ");\n";
if (parse_material_type(line)=='Textured') return [textured_material,texcoord_normals_select];

return [phongmaterial,normals_select];
}
function create_script(filename, contents, filenames)
{
  var res = "";

  if (g_path!="") {
    filename = g_path + "/" + filename;
  }



  var base_dir = get_base_dir(filename);
  var mtl_name = find_mtl_name(filenames);

  var material_value = get_material_value();
  var material = get_material(material_value, filename);

  var background_value = get_background_value();
  var background = get_background(background_value);

  var border_value = get_border_value();
  var border = get_border(border_value,material_value,filename);

  if (filename.substr(-4)==".stl") { res+="P I17=ev.polygon_api.stl_load(" + filename + ");\nP I18=ev.polygon_api.recalculate_normals(I17);\nP I19=ev.polygon_api.color_from_normals(I18);\nP I155=ev.polygon_api.color_grayscale(I19);\n";
     } else
  if (filename.substr(-4)==".obj") {
     if (mtl_name=="") {
       res+="P I155=ev.polygon_api.p_url(ev," + filename + ",350);\n";
       } else {
       res+="P I155=ev.polygon_api.p_mtl(ev," + filename +"," + mtl_name +"," + base_dir + ",600);\n";
       }
     } else
  if (filename.substr(-3)==".ds") { res+="P I155=ev.polygon_api.p_url(ev," + filename + ",350);\n"; } else
  //if (filename.substr(-4)==".ply") { res+="P I155=ev.points_api.ply_faces(" + filename + ");\n"; } else
  if (filename.substr(-4)==".zip") {
     res+="TF I154=ev.mainloop_api.gltf_load_sketchfab_zip("+filename+");\n"
     res+="P I1550=ev.mainloop_api.gltf_mesh_all_p(ev,I154);\n";
     res+="P I155=ev.polygon_api.or_array3(std::vector<P>{I1550});\n";
     res+="ML I62=ev.mainloop_api.gltf_mesh_all(ev,I154,0.90);\n"; // 0.75
  } else
  if (filename.substr(-4)==".glb") {
     res+="TF I154=ev.mainloop_api.gltf_loadKK("+base_dir+","+filename+");\n"
     res+="P I1550=ev.mainloop_api.gltf_mesh_all_p(ev,I154);\n";
     //res+="P I1550=ev.polygon_api.gltf_load(ev,I154,0,0);\n";
     res+="P I155=ev.polygon_api.or_array3(std::vector<P>{I1550});\n";
     res+="ML I62=ev.mainloop_api.gltf_mesh_all(ev,I154,0.9);\n";
  } else
  if (filename.substr(-5)==".gltf") {
    res+="TF I154=ev.mainloop_api.gltf_loadKK("+base_dir+","+filename+");\n"
     //res+="P I1550=ev.polygon_api.gltf_load(ev,I154,0,0);\n";
     res+="P I1550=ev.mainloop_api.gltf_mesh_all_p(ev,I154);\n";
     res+="P I155=ev.polygon_api.or_array3(std::vector<P>{I1550});\n";
     res+="ML I62=ev.mainloop_api.gltf_mesh_all(ev,I154,0.9);\n";

} else
     {
	res+="P I155=ev.polygon_api.cube(-300,300,-300,300,-300,300);\n";
	}


  var normals_val = get_normals_value();
  var border_color = "000000";
  var border_width = "1.0";
  var brd = get_border_value();
  if (brd>=0 && brd<store.state.border_db.length) {
     var name2 = store.state.border_db[brd];
     border_color = parse_border_color(name2);
     border_width = parse_border_width(name2);
  }
  if (border_width=="0") border_width="1.0";
if (normals_val==3)
  { // wireframe
  res+= "MT I4=ev.materials_api.m_def(ev);\n"

res+="P I1=ev.polygon_api.get_face_count(I155);\n";

res+="LI I433=ev.lines_api.from_polygon(I1);\n";
res+="P I633=ev.polygon_api.line_to_cone(ev,I433," + border_width +"/2,5);\n";
res+="P I733=ev.polygon_api.color(I633,ff" + border_color + ");\n";
res+="ML I66=ev.polygon_api.render_vertex_array_ml2(ev,I733);\n";

  } else
  if (normals_val==4)
  { // wireframe with hidden surface removal

  res+= "MT I4=ev.materials_api.m_def(ev);\n"
   res+="P I1=ev.polygon_api.get_face_count(I155);\n";
res+="P I114=ev.lines_api.p_towards_normal(I1,0.02);\n";
res+="P I124=ev.lines_api.p_towards_normal(I1,-0.02);\n";
res+="LI I115=ev.lines_api.from_polygon(I114);\n";
res+="LI I116=ev.lines_api.from_polygon(I124);\n";

res+="P I633=ev.polygon_api.line_to_cone(ev,I115," + border_width +"/2,5);\n";
res+="P I643=ev.polygon_api.line_to_cone(ev,I116," + border_width +"/2,5);\n";
res+="P I733=ev.polygon_api.color(I633,ff" + border_color + ");\n";
res+="P I743=ev.polygon_api.color(I643,ff" + border_color + ");\n";
res+="ML I135=ev.polygon_api.render_vertex_array_ml2(ev,I733);\n";
res+="ML I136=ev.polygon_api.render_vertex_array_ml2(ev,I743);\n";

var color = "000000";
var bg = get_background_value();
if (bg>=0&&bg<store.state.background_db.length) {
  var name2 = store.state.background_db[bg];
  var name = parse_bg_colour(name2);
  color = name;
  }

res+="P I145=ev.polygon_api.color(I155,ff" + color + ");\n";
res+="ML I156=ev.polygon_api.render_vertex_array_ml2(ev,I145);\n";
res+="ML I66=ev.mainloop_api.array_ml(ev,std::vector<ML>{I136,I135,I156});\n";
  } else {
   res+="P I1=ev.polygon_api.get_face_count(I155);\n";

  if (material[1]!="") {
     res+=material[1];
  } else
  if (!((filename.substr(-4)==".obj"&&mtl_name!="")||filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip")) {
      res+="P I2=ev.polygon_api.recalculate_normals(I1);\n";
      }
  res+="MT I3=ev.materials_api.m_def(ev);\n";
  if (material[0]!="") {
     res+=material[0];
  } else
  if (filename.substr(-4)==".obj"&&mtl_name!="") {
     res+="MT I4=ev.materials_api.texture_many2(ev,0.5);\n"
  } else
  if (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip") {
     res+="MT I4=ev.materials_api.gltf_material(ev,I154,0,1);\n";
     //res+="MT I4=ev.materials_api.gltf_anim_material2(ev,I154,0,30,I40,cvbnmfghjk);\n"
  } else {
     res+="MT I4=ev.materials_api.vertex_phong(ev,I3,-0.3,0.3,-1.0,ff888888,ffffffff,5.0,0.5);\n";
  }

 if ((!(filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip"))||material_value!=-1) { 
    res+="ML I62=ev.materials_api.bind(I2,I4);\n";
    }

    res+="P I771=ev.polygon_api.cube(-300.0,300.0,-300.0,300.0,-300.0,300.0);\n";
    res+="MT I772=ev.mainloop_api.mainloop_material(ev,I62);\n";
    //res+="MT I773=ev.materials_api.m_def(ev);\n"
    res+="MT I774=ev.materials_api.phong(ev,I772,0.0,0.0,1.0,ffffccaa,fffff8ee,30.0);\n";
    res+="MT I775=ev.materials_api.combine_materials(ev,I774,I774);\n"
    res+="ML I63=ev.materials_api.bind(I771,I775);\n";


  //res+="ML I64=ev.mainloop_api.depthmask(I63,false);\n";
res+="ML I6=ev.mainloop_api.depthfunc(I63,0);\n";
 
  //console.log(material_value);
  //console.log(border_value);
  //console.log(filename.substr(-4));
  //console.log(filename.substr(-5));
  if ((parseInt(material_value)==-1&&parseInt(border_value)==0) && (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip")) {
  //   res+="ML I6=ev.mainloop_api.gltf_mesh_all(ev,I154,0.5);\n";

  }


  if (normals_val==5||normals_val==6) { // wireframe

  res+="LI I433=ev.lines_api.from_polygon(I1);\n";
  res+="P I633=ev.polygon_api.line_to_cone(ev,I433," + border_width +"/2,5);\n";
  res+="P I733=ev.polygon_api.color(I633,ff" + border_color + ");\n";
  res+="ML I502=ev.polygon_api.render_vertex_array_ml2(ev,I733);\n";


  } else {
      res+=border; // outputs I502
  }


  res+="ML I66=ev.mainloop_api.array_ml(ev,std::vector<ML>{I502,I6});\n";
   }

  if (filename.substr(-4)==".glb" || filename.substr(-5)==".gltf"||filename.substr(-4)==".zip") {
    res+="ML I88=ev.mainloop_api.async_gltf(I66,I154);\n";
    res+="ML I89=ev.mainloop_api.mouse_roll_zoom2(ev,I88);\n";
    res+="ML I800=ev.mainloop_api.touch_rotate(ev,I89,true,true,0.01,0.01);\n";
    res+="ML I8=ev.mainloop_api.disable_polygons(I800);\n";



  } else {
    res+="ML I67=ev.mainloop_api.mouse_roll_zoom2(ev,I66);\n";
    res+="ML I800=ev.mainloop_api.touch_rotate(ev,I67,true,true,0.01,0.01);\n";
    res+="ML I8=ev.mainloop_api.disable_polygons(I800);\n";

    //res+="P I771=ev.polygon_api.cube(-100.0,100.0,-100.0,100.0,-100.0,100.0);\n";
    //res+="MT I772=ev.mainloop_api.mainloop_material(ev,I801);\n";
    //res+="MT I773=ev.materials_api.phong(ev,I772,0.0,0.0,1.0,ffff8822,ffff8822,30.0);\n";
    //res+="ML I8=ev.materials_api.bind(I771,I773);\n";


  }
  res+="ML I9=ev.mainloop_api.right_mouse_pan(ev,I8);\n";
  res+=background;
  res+="ML I14=ev.mainloop_api.array_ml(ev,std::vector<ML>{I44,I9});\n";
  res+="RUN I10=ev.blocker_api.game_window2(ev,I14,false,false,0.0,1000000.0);\n";


return res;  
}
function allowDrop(ev)
{
  ev.preventDefault();
}
function preventDefault(ev)
{
   if (ev.button==2)
      ev.preventDefault();
}

function publish_face_count(state)
{
   if (Module)
   {
     var value = Module.ccall('get_integer', 'number', ['number'],[0]);
     if (value != 0) {
     //var elem = document.getElementById("modelinfo");
     //if (elem) elem.innerHTML = "faces: " + value;
     state.model_info = value + " faces";
     }
   }
}

var contents_array = [];
var filename_array = [];
var g_filename = "";
var g_path = "";

function set_model_info(state,val)
{
 state.model_info = val;
}

function set_filename_info(state,filename)
{
   let res = "";
   if (filename) {
   var pos = filename.lastIndexOf('/');
   if (pos==-1)
      res = filename;
   else
      res = filename.substr(pos+1);   
   }
   if (res.length>13) {
      if (res.length>16) {
      	  state.appmodel_is_twoline = "2";
      } else {
      	state.appmodel_is_twoline = "1";
      }
      } else {
      state.appmodel_is_twoline = "0";
      }
   if (res.length>16) {
      var res1 = res.substring(0,30);
      var res2 = res.substring(30);
      state.filename1 = res1;
      state.filename2 = res2;
      state.filename = "";
   } else {
     state.filename = res;
     state.filename1 = "";
     state.filename2 = "";
     }
}

function extract_contents(state,file_array,filenames, filename, path)
{
   g_path = path;
   g_filename = filename;
   set_filename_info(state,g_filename);
   state.url = g_path;
   if (file_array==""||filenames=="") {
      return new Promise((resolve,reject) => {
           resolve("success");
      });
   }
   if (path!="") {

   return new Promise((resolve,reject) => {  

   contents_array = [];
   filename_array = [];
   var s = file_array.length;
   var counter = 0;
   for(var i = 0;i<s;i++) {
     var file = file_array[i];
     let fileReader = new FileReader();
     var f = (fileReader,i) => {
     fileReader.onload = () => {
        let fileContents = fileReader.result;
	var binary = '';
	var bytes = new Uint8Array( fileContents );
	//console.log(bytes.length);
	var len = bytes.byteLength;
	for(var j=0;j<len;j++) {
	  binary+= String.fromCharCode(bytes[j]);
	}
	//console.log(binary.length);
	contents_array.push(binary);
	filename_array.push(fix_filename(filenames[i]));
	counter++;
	if (counter==s) resolve("success");
	}
     };
     f(fileReader,i);
     fileReader.readAsArrayBuffer(path+"/"+file);
  }
});



   }


   return new Promise((resolve,reject) => {  

   contents_array = [];
   filename_array = [];
   var s = file_array.length;
   var counter = 0;
   for(var i = 0;i<s;i++) {
     var file = file_array[i];
     let fileReader = new FileReader();
     var f = (fileReader,i) => {
     fileReader.onload = () => {
        let fileContents = fileReader.result;
	var binary = '';
	var bytes = new Uint8Array( fileContents );
	//console.log(bytes.length);
	var len = bytes.byteLength;
	for(var j=0;j<len;j++) {
	  binary+= String.fromCharCode(bytes[j]);
	}
	//console.log(binary.length);
	contents_array.push(binary);
	filename_array.push(fix_filename(filenames[i]));
	counter++;
	if (counter==s) resolve("success");
	}
     };
     f(fileReader,i);
     fileReader.readAsArrayBuffer(file);
  }
});
}
var loading_data = 0;

function load_finished(value)
{
   //console.log("LOAD FINISHED");
   load_files(contents_array,filename_array);
   load_emscripten(store.state,g_filename, contents_array, filename_array);
   set_label("Load finished..");
}
function fix_filename(filename)
{
  var res = "";
  var s = filename ? filename.length : 0;
  for(var i=0;i<s;i++) {
     if (filename[i]==' ') res+="_"; else 
     if (filename[i]=='\\') res+="/"; else res+=filename[i];
  }
  return res;
}
function traverseFileTree(item, path) {
   return new Promise( resolve => {
   path = path || "";
   if (item.isFile) {
      item.file(function(file) {
         //console.log("File:", path + file.name);
	 resolve({file: file,path: path+file.name});
	 });
    } else if (item.isDirectory) {
          var res = [];
	  var names = [];
	  var dirReader = item.createReader();
	  dirReader.readEntries(function(entries) {
	     var promises = [];
	     for(var i=0;i<entries.length;i++) {
	        const promise = traverseFileTree(entries[i],path+item.name + "/");
		promises.push(promise);		
		}
		resolve(Promise.all(promises));
		});
	}
	});
}
function flatten_arrays(data)
{
   var res = [];
   var names = [];
   if (Array.isArray(data)) {
     var s = data.length;
     for(var i=0;i<s;i++) {
        const [res2,names2] = flatten_arrays(data[i]);
	for(var k=0;k<res2.length;k++) {
	   res.push(res2[k]);
	   names.push(names2[k]);
	}
     }
     return [res,names];
   }
   return [[data.file],[data.path]];
}
var old_main_item_dir = "";
var old_main_item_name = "";
var old_files = "";
var old_filenames = "";
function drop2(state)
{
   set_filename_info(state,"");
     set_model_info(state,"(loading..)");
  var model_val = get_model_value();
  var model = get_model(model_val);
  var url = state.url;
  var path = "";
  if (url!="") {
     let result = url.lastIndexOf("/");
     path = url.substr(0,result);
     model = url.substr(result+1);
  }
  if (old_main_item_name!="" || model!="") {
    if (path!="") { old_main_item_dir=path; }
    if (model!="") { old_main_item_name=model; }
    // old_files, old_filenames
    if (state) { state.filename = fix_filename(old_main_item_name); }
    const promise = extract_contents(state,"","",fix_filename(old_main_item_name),path);
    promise.then(load_finished);
    }
}

function drop3(state,selectfileelem)
{
   set_filename_info(state,"");
   var elem2 = document.getElementById(selectfileelem);
   var files2 = elem2.files;

   //console.log(files2);
   set_model_info(state,"(loading..)");
  set_label("Loading model..");

  //var mod = document.getElementById("model-select");
  //mod.value="-1";


  var files = [];
  var filenames = [];
  if (files2) {
     const promise2 = new Promise( resolve => {

     	   var s = files2.length;
     	   for(var i=0;i<s;i++) {
	   	   //console.log(files2[i].name);
	   	   filenames.push(files2[i].name);
		   files.push(files2[i]);
	   }

           //const [files,filenames] = flatten_arrays(data);
	   //console.log(files);
	   //console.log(filenames);
           var main_item_num = find_main_item(filenames);
	   var main_item = files[main_item_num];
           var main_item_name = filenames[main_item_num];

/*
	   var snd_item_num = -1;
	   var snd_item_name = "";
	   //console.log(main_item_name);
	   if (main_item_name.substr(-4)==".zip") {
	      console.log("ZIP UNCOMPRESS");
	      var zipentries=get_zip_filenames(main_item);
	      snd_item_num = find_main_item(zipentries);
	      snd_item_name = zipentries[snd_item_num];
	      console.log(snd_item_num);
	      console.log(snd_item_name);
	   }
	   */


	   old_files = files;
	   old_filenames = filenames;
	   old_main_item_name = main_item_name;
	   /*
	   old_snd_item_num = snd_item_num;
	   old_snd_item_name = snd_item_name;
*/

           const promise = extract_contents(state,files,filenames,fix_filename(main_item_name),"");
           promise.then(load_finished);

     });
     }


}

function drop(ev)
{
  set_model_info(store.state,"(loading..)");
  set_label("Loading model..");

  var mod = document.getElementById("model-select");
  if (mod) mod.value="-1";

  ev.preventDefault();

  var files = [];
  var filenames = [];
  if (ev.dataTransfer.items) {
     const promise2 = new Promise( resolve => {

     	   var s = ev.dataTransfer.items.length;
	   var promises = [];
     	   for(var i=0;i<s;i++) {
       	   	   var item = ev.dataTransfer.items[i].webkitGetAsEntry();
       		   if (item) {
		   promises.push(traverseFileTree(item));
	  	   }
		   }
	    resolve(Promise.all(promises));
       });
       promise2.then( data => {
           const [files,filenames] = flatten_arrays(data);
	   //console.log(files);
	   //console.log(filenames);
           var main_item_num = find_main_item(filenames);
	   var main_item = files[main_item_num];
           var main_item_name = filenames[main_item_num];

/*
	   var snd_item_num = -1;
	   var snd_item_name = "";
	   console.log(main_item_name);
	   if (main_item_name.substr(-4)==".zip") {
	      console.log("ZIP UNCOMPRESS");
	      var zipentries=get_zip_filenames(main_item);
	      snd_item_num = find_main_item(zipentries);
	      snd_item_name = zipentries[snd_item_num];
	      console.log(snd_item_num);
	      console.log(snd_item_name);
	   }
*/

	   old_files = files;
	   old_filenames = filenames;
	   //console.log(filenames);

	   old_main_item_name = main_item_name;
	   
/*	   old_snd_item_num = snd_item_num;
	   old_snd_item_name = snd_item_name;
	   */
           const promise = extract_contents(store.state,files,filenames,fix_filename(main_item_name),"");
           promise.then(load_finished);

     });
     }
}

</script>

<script>
var canv = document.getElementById("canvas");
var Module = {
   canvas : canv,
   arguments : [ "--size", "800", "600", "--code", default_script(), "--homepage", "https://tpgames.org/", "--href", window.location.href],
   print : (function() { return function(text) { console.log(text); } })(),
   printErr : (function() { return function(text) { console.log(text); } })(),
   };


function load_emscripten(state,filename, contents, filenames)
{
    var data2 = "<?php echo $date ?>";
    var agent = navigator.userAgent;
    var mobile = false;
    var firefox = false;
    var idx = 0;
    if (agent.indexOf("Mobile") != -1) mobile = true;
    if ((idx=agent.indexOf("Firefox")) != -1) firefox = true;

    var src = "web_page_highmem.js?"+data2;
    var vstr = agent.substring(idx+8);
    var vnum = parseInt(vstr);

    if (firefox && vnum<=78)
	src="web_page_nothreads.js?" + data2;
    else if (firefox) src="web_page_nothreads_highmem.js?" + data2;
    if (mobile) src="web_page_lowmem.js?"+data2;
    if (!g_emscripten_running) {
      if (filename=="") {
      Module.arguments = [ "--size", "800", "600", "--code", default_script(), "--homepage", "https://tpgames.org/", "--href", window.location.href];
      } else {
      Module.arguments = [ "--size", "800", "600", "--code", convert_enter_to_at(create_script(filename,contents,filenames)), "--homepage", "https://tpgames.org/", "--href", window.location.href];
      }

      var script = document.createElement("script");
      script.setAttribute("src", src);
      document.getElementsByTagName("head")[0].appendChild(script);
      check_if_emscripten_running();
      } else {
      Module.ccall('set_string', null, ['number', 'string'],[0,create_script(filename,contents,filenames)]);
      }
      setTimeout(function() { check_emscripten_ready(state) }, 100);
}

function load_files(data_array, filename_array)
{
  var s2 = data_array.length;
  for(var i=0;i<s2;i++) {
    var data3 = data_array[i];


    var filename = filename_array[i];
    var sz = data3.length;

    Module.ccall('set_string', null, ['number', 'string'], [1,filename], {async:true} );

    for(var s=0;s<sz;s+=40960) {
      var use_sz = 40960;
      if (s>=sz-40960) {
        use_sz=sz-s;
      }
    Module.ccall('set_integer', null, ['number', 'number'], [2,use_sz], {async:true} );

    var buffer = "";
    var index = "0123456789abcdef";
    for(var o = 0;o<use_sz;o++) {
        var low = data3[s+o].charCodeAt(0)&0x0f;
	var high = (data3[s+o].charCodeAt(0)&0xf0)>>>4;
	buffer+=index[high];
	buffer+=index[low];
    }
    //for(var c = 0;c<15;c++) console.log(buffer[c]);
    const uint8 = intArrayFromString(buffer,false);
    
    

    Module.ccall('set_string', null, ['number', 'array'], [3,uint8], {async:true} );
    }
    Module.ccall('set_string', null, ['number', 'string'], [4,""], {async:true} );

  }
}


var g_emscripten_running = false;
function check_em() {
    return function() {
	g_emscripten_running = true;
	//console.log("EMSCRIPTEN RUNNING");
	//resize_event(null);
	//load_file();
	load_data();
	if (loading_data==1) {
	//console.log("LOADING DATA");
	app.change_appmodel(1);
	app.change_category();
	setTimeout(function() {
	//console.log("TIMEOUT");
	load_data();
	app.change_model();
	load_finished(1);
	}, 100);

   set_filename_info(app.state,g_filename);
   }
set_label("Drag & Drop files..");
    }
}

function check_emscripten_running()
{
    var canv = document.getElementById("canvas");
    if (Module) {
	Module['onRuntimeInitialized'] = check_em();
    } else {
	setTimeout(function() { check_emscripten_running() }, 100);
    }
}
function check_if_emscripten_running()
{
  setTimeout(function() { check_emscripten_running() },100);
}
function emscripten_loading_callback()
{
   //console.log("Preparing");
   set_label("Preparing..");
}
function emscripten_ready_callback(state)
{
   //console.log("Ready");
   //app.methods.change_appmodel(0);
   if (store.state.appmodel_is_loading == "true") {
      store.state.appmodel_is_loading = "false";
      store.state.appmodel_is_notselected = "true";
      }
   set_label("Ready..");
   publish_face_count(state);
}
function check_emscripten_ready(state)
{
  if (g_emscripten_running && Module) {
    var value = Module.ccall('get_integer', 'number', ['number'],[1]);
    //if (value !== undefined)
    //    console.log('ready: ' . value);
    if (value==2) { emscripten_loading_callback();
    	 setTimeout(function() { check_emscripten_ready(state) }, 100);
    } else
    if (value==1) { emscripten_ready_callback(state); }
    else {
    	 setTimeout(function() { check_emscripten_ready(state) }, 100);
	 }
	 } else
	 {
    	 setTimeout(function() { check_emscripten_ready(state) }, 100);
	 }
}
function clickselectfile2()
{
  var elem = document.getElementById("selectfile2");
  elem.click();
}
function clickselectfile()
{
  var elem = document.getElementById("selectfile");
  elem.click();
}
function set_label(label)
{
   var wid = document.getElementById("label");
   if (wid) wid.innerHTML = label;
}
set_label("Loading 3d engine..");
if (store) {
   load_emscripten(store.state,"");
   }

window.onresize = resize_event;
window.setTimeout(function() { resize_event(null); },10);

function resize_event(event)
{
  var wd = window.innerWidth;
  var hd = window.innerHeight;

  var mobile = false;
  if (navigator.userAgentData!=null && navigator.userAgentData.mobile==true) {
     mobile=true;
     var d = document.getElementById("dp");
     d.style="display:block";
     var d2 = document.getElementById("lab");
     d2.style="width:100%";
  }

  wd-=120;
  

  var w = 800;
  var h = 600;

  wd/=3.0;
  wd*=2.0;
  
  hd/=10.0;
  hd*=7.5;

  var s_x = wd/w;
  var s_y = hd/h;

  var scale;
  if (s_x<s_y) scale=s_x; else scale=s_y;

  if (mobile) { scale=wd*3.5/2/w; }

  var scale_x = w*scale;
  var scale_y = h*scale;


  var elem = document.getElementById("canvas");
  elem.style.width = scale_x.toString() + "px";
  elem.style.height = scale_y.toString() + "px";

}

function serialize_state()
{
   var elem = document.getElementById("category-select");
   var elem2 = document.getElementById("metal-type-select");
   var elem3 = document.getElementById("plastic-type-select");
   var elem4 = document.getElementById("textured-type-select");

   var ser = {
          model   : get_model_value(),
          normals : get_normals_value(),
          border  : get_border_value(),
        background: get_background_value(),
         material : get_material_value(),
	 category : elem && elem.value,
	 metal : elem2 && elem2.value,
	 plastic : elem3 && elem3.value,
	 textured : elem4 && elem4.value
   };
   return JSON.stringify(ser);
}

function deserialize_state(txt)
{
  //console.log("STATE");
  var ser = JSON.parse(txt);
  var model = ser.model;
  var normals = ser.normals;
  var border = ser.border;
  var bg = ser.background;
  var cat = ser.category;
  var metal = ser.metal;
  var plastic = ser.plastic;
  var textured = ser.textured;
  //var mat = ser.material;

  var elem5 = document.getElementById("category-select");
  if (elem5) elem5.value = cat;
  app.change_category();

  var elem = document.getElementById("model-select");
  if (elem) elem.value = model;
  var elem2 = document.getElementById("normals-select");
  if (elem2) elem2.value = normals;
  var elem3 = document.getElementById("border-select");
  if (elem3) elem3.value = border;
  var elem4 = document.getElementById("background-select");
  if (elem4) elem4.value = bg;
  var elem6 = document.getElementById("metal-type-select");
  if (elem6) elem6.value = metal;
  var elem7 = document.getElementById("plastic-type-select");
  if (elem7) elem7.value = plastic; else { /*console.log("PLASTIC ERROR");*/ }
  var elem8 = document.getElementById("textured-type-select");
  if (elem8) elem8.value = textured;
}

function load_data()
{
   var st = document.getElementById("formstate2");
   var ca = document.getElementById("formcontentsarray2");
   var fa = document.getElementById("formfilenamearray2");
   var gf = document.getElementById("formgfilename2");
   var gp = document.getElementById("formgpath2");
   //console.log(st.textContent.length);
   //console.log(ca.textContent.length);
   //console.log(fa.textContent.length);
   //console.log(gf.textContent.length);
   //console.log(gp.textContent.length);
   if (st.textContent != "") {
    loading_data=1;
   var a_st = "";
   if (st.textContent !="")
      a_st = st.textContent;
   //console.log(ca.textContent);
   var a_ca = "";
   //console.log("CONTENTSARRAY");
   if (ca.textContent !="")
      a_ca = JSON.parse(ca.textContent);
   //console.log(fa.textContent);
   //console.log("FILENAMEARRAY");
   var a_fa = "";
   if (fa.textContent !="")
      a_fa = JSON.parse(fa.textContent);
   //console.log(gf.textContent);
   //console.log("FILENAME");
   var a_gf = "";
   if (gf.textContent != "")
      a_gf = JSON.parse(gf.textContent);
   //console.log(gp.textContent);
   // console.log("GPATH");
  var a_gp = "";
   if (gp.textContent != "") 
      a_gp = JSON.parse(gp.textContent);

   deserialize_state(a_st);
   contents_array = base64_to_array(a_ca);
   filename_array = base64_to_array(a_fa);
   g_filename = a_gf;
   g_path = a_gp;

/*
   var snd_item_num = -1;
   var snd_item_name = "";
   console.log(g_filename);
   if (g_filename.substr(-4)==".zip") {
      var idx = find_main_item(filename_array);
      var zip_file = contents_array[idx];
      console.log("ZIP UNCOMPRESS3");
      var zipentries = get_zip_filenames(zip_file).then(function(value) {
      console.log(value);
	snd_item_num = find_main_item(value);
      	snd_item_name = zipentries[snd_item_num];
	old_snd_item_num = snd_item_num;
   	old_snd_item_name = snd_item_name;
      console.log(snd_item_num);
      console.log(snd_item_name);
      }, function(error) { console.log("ERROR"); console.log(error); });
   }
*/

   old_files = contents_array;
   old_filenames = filename_array;
   old_main_item_name = g_filename;
   old_main_item_dir = g_path;
   }
}

var Base64={
    _keyStr:"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",
     encode:function(e){
	var t="";
	var n,r,i,s,o,u,a;
	var f=0;
	e=Base64._utf8_encode(e);
	while(f<e.length){
		n=e.charCodeAt(f++);
		r=e.charCodeAt(f++);
		i=e.charCodeAt(f++);
		s=n>>2;
		o=(n&3)<<4|r>>4;
		u=(r&15)<<2|i>>6;
		a=i&63;
		if(isNaN(r)){u=a=64;}else
		if(isNaN(i)){a=64;}
		t=t+this._keyStr.charAt(s)+this._keyStr.charAt(o)+this._keyStr.charAt(u)+this._keyStr.charAt(a);}
		return t;
		},
	decode:function(e){
	    var t="";
	    var n,r,i;
	    var s,o,u,a;
	    var f=0;
	    e=e.replace(/[^A-Za-z0-9+/=]/g,"");
	    while(f<e.length){
	       s=this._keyStr.indexOf(e.charAt(f++));
	       o=this._keyStr.indexOf(e.charAt(f++));
	       u=this._keyStr.indexOf(e.charAt(f++));
	       a=this._keyStr.indexOf(e.charAt(f++));
	       n=s<<2|o>>4;
	       r=(o&15)<<4|u>>2;
	       i=(u&3)<<6|a;
	       t=t+String.fromCharCode(n);
	       if(u!=64){t=t+String.fromCharCode(r);}
	       if(a!=64){t=t+String.fromCharCode(i);}
	       }
	       t=Base64._utf8_decode(t);
	       return t;
	       },
	_utf8_encode:function(e){
	    e=e.replace(/rn/g,"n");
	    var t="";
	    for(var n=0;n<e.length;n++){
	       var r=e.charCodeAt(n);
	       if(r<128){t+=String.fromCharCode(r);}
	       else if(r>127&&r<2048){
	           t+=String.fromCharCode(r>>6|192);
		   t+=String.fromCharCode(r&63|128);
		   }
		   else
		   {
		   t+=String.fromCharCode(r>>12|224);
		   t+=String.fromCharCode(r>>6&63|128);
		   t+=String.fromCharCode(r&63|128);
		   }
		   }
		   return t;},
       _utf8_decode:function(e){
           var t="";
	   var n=0;
	   var r=c1=c2=0;
	   while(n<e.length){
		r=e.charCodeAt(n);
		if(r<128){
			t+=String.fromCharCode(r);
			n++
		}else if(r>191&&r<224){
		   c2=e.charCodeAt(n+1);
		   t+=String.fromCharCode((r&31)<<6|c2&63);
		   n+=2
		}else{
		   c2=e.charCodeAt(n+1);
		   c3=e.charCodeAt(n+2);
		   t+=String.fromCharCode((r&15)<<12|(c2&63)<<6|c3&63);n+=3
		   }}
	return t;}}

function array_to_base64(arr)
{
   var res = [];
   for(var i=0;i<arr.length;i++)
   {
     var val = arr[i];
     var buffer = btoa(val); //Base64.encode(val);
     res.push(buffer);
     
   }
   return res;
}
function base64_to_array(arr)
{
   var res = [];
   for(var i=0;i<arr.length;i++) {
     var val = arr[i];
     var res2 = atob(val); //Base64.decode(val);
     res.push(res2);
   }
   return res;
}

function hash(val)
{
	return ((val*10001) ^ 0x26522663).toString();
}

function submitprogressbar(i)
{
   var prog = document.getElementById("progressbar");
   prog.innerHTML = "<progress value='"+ i.toString() + "' max='500'></progress>";
   //for(var ii=0;ii<i;ii++) {
   //prog.innerHTML = prog.innerHTML + "&exist;";
   //}
   //for(var ik=i;ik<10;ik++)
   //{
   //prog.innerHTML = prog.innerHTML + "_";   
   //}
   if (i==500)
   {
	var name = "viewdata/num.txt";
	fetch(name).then(response => {
	   response.body.getReader().read().then(value => {
	   var str = strfy(value.value);
	   var num = parseInt(str);
	   prog.innerHTML = "<a href='https://meshpage.org/view.php?id=" + hash(num) + "'>https://meshpage.org/view.php?id=" + hash(num) + "</a>";
	   }
	   )});
   }
}


function formsubmit()
{

  //var st3 = document.getElementById("formcontentsarray");
  var at3 = JSON.stringify(array_to_base64(contents_array));
  //var st4 = document.getElementById("formfilenamearray");
  var at4 = JSON.stringify(array_to_base64(filename_array));
  //var st5 = document.getElementById("formnum");

  var contents_length = at3.length;
  var filename_length = at4.length;

  var data = new FormData();
  data.append("num",-1);
  var xhr = new XMLHttpRequest();
  xhr.open('POST','submit_contents.php', true);

  var submitprogress = 0;

  xhr.onload = function() {
  	     //console.log(contents_length);
	     //console.log(filename_length);
	     var num = 500;
	     var step = Math.floor(contents_length/num)+1;
	     var step2 = Math.floor(filename_length/num)+1;
	     if (step<1) step=1;
	     if (step2<1) step2=1;
  	     	 for(var i=0;i<num;i++) {

		   var start = i*step;
 		   var end = start + step;
		   if (i==num-1) end=contents_length;
		   if (start>contents_length) start=contents_length;
		   if (end>contents_length) end=contents_length;

  		   var start2 = i*step2;
  		   var end2 = start2 + step2;
		   if (i==num-1) end2=filename_length;
		   if (start2>contents_length) start2=contents_length;
		   if (end2>contents_length) end2=contents_length;

  		   var st3_sub = at3.substring(start, end);
  		   var st4_sub = at4.substring(start2, end2);

  		   var st = serialize_state();
  		   //var st2 = document.getElementById("formstate");
  		   //st2.value = st;

  		   //var st5 = document.getElementById("formgfilename");
  		   var st5_val = JSON.stringify(g_filename);
		   if (g_path=="") { g_path="./"; }
		   var st6_val = JSON.stringify(g_path);

  		   //var form = document.getElementById("submitcontents");
  		   //form.submit();

  		   var data = new FormData();
  		   data.append("state", st);
  		   data.append("contents_array", st3_sub);
		   //console.log("CONTENTSARRAYSIZE=");
		   //console.log(st3_sub.length);
  		   data.append("filename_array", st4_sub);
  		   data.append("g_filename", st5_val);
		   data.append("g_path", st6_val);
  		   data.append("num",i);
  		   var xhr2 = new XMLHttpRequest();
  		   xhr2.open('POST','submit_contents.php', true);	
		   submitprogressbar(0);
		   xhr2.onload = function()
		   {
			submitprogress=submitprogress + 1;
			submitprogressbar(submitprogress);			
			
		   }
  		   xhr2.send(data);
  		   }
		   }
		  
  xhr.send(data);


}

//var form = document.getElementById("submitcontents");
//form.addEventListener('submit', (event) => {
//  console.log("EVENTLISTENER");
  //event.preventDefault();

//});

</script>
