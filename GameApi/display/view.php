<?php

ini_set("post_max_size", "120M");
ini_set("upload_max_filesize", "100M");
ini_set("memory_limit", "1024M");
//header("Access-Control-Allow-Headers: Range");
include("backend.php");
//header("Cross-Origin-Opener-Policy: same-origin");
$date = filemtime("engine_highmem.js");

$machine=php_uname("n");
$siteprefix = "";
if ($machine=="terop-pc2") {
   $site = "meshpage.org";
   $assetsite = "meshpage.org/assets";
   $sitename = "meshpage.org";
   $siteprefix=$_SERVER['HTTP_HOST'];
   $siteprefix=substr($siteprefix,0,4);
   if ("$siteprefix"!="ssh.") $siteprefix="";   
   } else {
   $site = "https://dinoengine.com";
   $assetsite = "https://dinoengine.com/assetsite";
   $sitename = "dinoengine.com";
   }
   $site = "https://" . $siteprefix . $site;
   $assetsite = "https://" . $siteprefix . $assetsite;


function unhash($data)
{
   return ((intval($data)) ^ 0x26522663) /10001;
}
$iid = $_GET["id"];
$id=unhash($_GET["id"]);

//echo "ID=$id";
$user = "terop";
/*
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
*/
?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, shrink-to-fit=no"/>
<!--link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.4.1/dist/css/bootstrap.min.css" integrity="sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh" crossorigin="anonymous"-->
</head>
<body>
<script>
setInterval(() => {
  console.log(
    wasmMemory.buffer.byteLength / 1024 / 1024,
    "MB"
  );
}, 1000);

function json_stringify(val)
{
   return JSON.stringify(val);
}

function json_parse(val)
{
   return JSON.parse(val);
}


var agent = navigator.userAgent;
var mobile = false;
var firefox = false;
if (agent.indexOf("Mobile") != -1) mobile=true;
if ((idx=agent.indexOf("Firefox")) != -1) firefox=true;
var firefox_version = parseInt(agent.substring(idx+8));

var req = new XMLHttpRequest();
var download_result={};
var g_download_done = false;
req.onload = function() {
   //console.log(this.responseText);
   download_result=json_parse(this.responseText);
   g_download_done = true;
}
req.open("get", "view_fetch.php?id=<?php echo $id ?>", true);
req.send();

</script>
<?php

//echo "<pre id='formstate2' style='display:none'>";
//echo "$state";
//echo "</pre>";

//echo "<pre id='formcontentsarray2' style='display:none'>";
//echo "$contentsarray";
//echo "</pre>";

//echo "<pre id='formfilenamearray2' style='display:none'>";
//echo "$filenamearray";
//echo "</pre>";

//echo "<pre id='formgfilename2' style='display:none'>";
//echo "$gfilename";
//echo "</pre>";

//echo "<pre id='formgpath2' style='display:none'>";
//echo "$gpath";
//echo "</pre>";



?>

<script src="https://cdn.jsdelivr.net/npm/vue@1.0.28/dist/vue.js"></script>
<script>
if (!crossOriginIsolated) {
    console.log("NOT CROSSORIGIN ISOLATED => running in lowmem/nothreads mode");
    console.log("Your web server needs the following configuration to get gameapi builder animations working:");
    console.log("Header set Access-Control-Allow-Origin https://meshpage.org");
console.log("Header set Cross-Origin-Embedder-Policy \"require-corp\"");
console.log("Header set Cross-Origin-Resource-Policy \"same-site\"");
console.log("Header set Cross-Origin-Opener-Policy \"same-origin\"");
console.log("Header set Access-Control-Allow-Headers \"Range\"");
console.log("NOTE: you should change https://meshpage.org to your own web hosting location");

}

</script>

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
	  v-bind:is_model_loading="state.appmodel_is_model_loading"
	  v-bind:is_twoline="state.appmodel_is_twoline"
	  v-bind:model_info="state.model_info"
	  v-bind:progress_1="state.progress_1"
	  v-bind:progress_2="state.progress_2"
	  v-bind:progress_3="state.progress_3"
	  v-on:examples_click="change_appmodel(2)"
	  v-on:link_click="change_appmodel(4)"
	  v-on:change_model="change_model()"
	  v-on:change_choose="change_model()"
	  v-on:change_choose_url="change_model_url()"
	  v-on:root_handle_drop="change_model3"
	  v-bind:filename="state.filename"
	  v-bind:filename1="state.filename1"
	  v-bind:filename2="state.filename2"
></appmodel>
<appbackground v-on:change_model="change_model()"
	       v-bind:state="state"></appbackground>
<objbrightness v-on:change_model="change_model()"
	       v-bind:state="state"></objbrightness>
<appmaterial v-on:change_model="change_model()"
	     v-on:change_category="change_category()"
	     v-bind:is_metal="state.is_metal"
	     v-bind:is_plastic="state.is_plastic"
	     v-bind:is_textured="state.is_textured"
	     v-bind:filter_material_metal="filter_material_metal()"
	     v-bind:filter_material_plastic="filter_material_plastic()"
	     v-bind:filter_material_textured="filter_material_textured()"
></appmaterial>
<appborder v-on:change_model="change_model()"
	   v-bind:state="state"></appborder>
<appnormals v-on:change_model="change_model()"></appnormals>
<appsubmitbutton></appsubmitbutton>
<br>
</div>
</div>
<div style="height:10px"></div>
<div style="height:10px"></div>
</appdragdroparea>
</div> <!-- vue ends here -->
Warning: If you submit content to using the submit button and the content is owned by someone else, you need to obtain reproduction rights and give meshpage.org permission to publish the content in meshpage.org. While the content is available only to people who gets access to resulting urls, the reproduction rights are still required.
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
      appmodel_is_model_loading: "false",
      appmodel_is_twoline: "0",
      progress_1: "0",
      progress_2: "0",
      progress_3: "0",
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
      brightness_db: [],
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
   methods: {
   parse_material_count: function(mat,type)
   {  
   var arr = mat.split(" ");
   return arr[0];
   },
   parse_material_name: function(mat, type) {
      return mat.replace(type + ' ', '');
   }
   },
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
		 </select>
		 </div>
		 <div v-show="is_metal=='true'">
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
		 <div v-show="is_plastic=='true'">
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

		 <div v-show="is_textured=='true'">
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
    props: ['state'],
    methods: {
      parse_bg_count: function(bg,type)
      {
       var arr = bg.split(" ");
       return arr[0];
      },
     parse_bg_name: function(bg,type)
     {
      var arr = bg.split(" ");
      return arr[1];
     }

    },
    template: `
    <div class="block blockitem height8 border customfont">
    Background<br>
    <div class="horizspace customfont">
    <select name="background" id="background-select" v-on:change="$emit('change_model')">
      <!--option value="-1">Black</option-->
    <template v-for="bg in state.background_db">
    <option v-bind:value="parse_bg_count(bg)">{{ parse_bg_name(bg) }}</option>
    </template>
    </select></div></div>
    `
});


Vue.component('objbrightness', {
  data: function() {
    return {
    }
    },
    props: ['state'],
    methods: {
      parse_bright_count: function(bg,type)
      {
       var arr = bg.split(" ");
       return arr[0];
      },
     parse_bright_name: function(bg,type)
     {
      var arr = bg.split(" ");
      return arr[1];
     }

    },
    template: `
    <div class="block blockitem height8 border customfont">
    Brightness<br>
    <div class="horizspace customfont">
    <select name="brightness" id="brightness-select" v-on:change="$emit('change_model')">
      <!--option value="-1">Black</option-->
    <template v-for="bg in state.brightness_db">
    <option v-bind:value="parse_bright_count(bg)">{{ parse_bright_name(bg) }}</option>
    </template>
    </select></div></div>
    `
});


Vue.component('appborder', {
  data: function() {
     return {
     }
     },
     methods: {
     	  parse_border_count: function(brd,type)
	  		      {
			         var arr = brd.split(" ");  
				    return arr[0];
				    },
	  parse_border_name: function(brd,type)
  	  		     {
			        var arr = brd.split(" ");
				   return arr[1] + " " + arr[2];
				   }

		},
     props: ['state'],
     template: `<div class="block blockitem height8 border customfont">
     Border<br><div class="horizspace customfont"><select name="border" id="border-select" v-on:change="$emit('change_model')">
  <!--option value="-1">None</option -->
  <template v-for="brd in state.border_db">
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
 <template v-for="model in state.model_db">
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
   <button type="button" v-on:click="$emit('change_model3')" style="margin-right:0; margin-left: auto; display: block; width: 80px; height: 30px">Open</button>

</div>
     `
     });

Vue.component('appmodel_loading', {
  data: function() {
     return { } },
     template: `<div class="border block blockitem height12 customfont prettysmall">
      Loading 3d engine...
      </div>`
      });
Vue.component('appmodel_model_loading', {
  data: function() {
     return { } },
     template: `<div class="border block blockitem height12 customfont prettysmall">
      Loading 3d model... 
      </div>`
      });

Vue.component('appmodel_notselected', {
   data: function() {
     return { } },
   methods: {
      onFileChange: function(event) { this.$emit('handle_drop', event); }
   },
   template: `<div class="border block blockitem height12 customfont">
   <small>Please Drag & Drop any 3D model to this page. You can also try our <a href="javascript:;" v-on:click="$emit('examples_click')">examples</a><!--or <a href="javascript:;" v-on:click="$emit('link_click')">link</a-->.</small>
   <br><br><small>.STL, .OBJ, .GLB, .ZIP, .VOX file types supported. See help for materials.</small>
   <button type="button" onclick="clickselectfile()" style="margin-right:0; margin-left: auto; display: block; width: 80px; height: 30px">Open</button>
   <input id="selectfile" type="file" multiple v-on:change="onFileChange" style="display:none"/>
   </div>`
   });

Vue.component('appmodel_selected', {
  props: ['filename', 'filename1', 'filename2', 'model_info', 'progress_1', 'progress_2', 'progress_3', 'is_twoline'],
  methods: {
     onFileChange: function(event) { this.$emit('handle_drop', event); }
  },
  data: function() {
    return { } },
    template: `<div class="border block blockitem height12 customfont">
      <!--appprogress></appprogress-->
      <appfilenameinfo v-bind:is_twoline="is_twoline" v-bind:filename="filename" v-bind:filename1="filename1" v-bind:filename2="filename2"></appfilenameinfo>
      <appmodelinfo v-bind:progress_1="progress_1" v-bind:progress_2="progress_2" v-bind:progress_3="progress_3" v-bind:model_info="model_info"></appmodelinfo>
      <button type="button" onclick="clickselectfile2()" style="margin-right:0; margin-left: auto; display: block; width: 80px; height: 30px">Open</button>
   <input id="selectfile2" type="file" multiple v-on:change="onFileChange" style="display: none;"/>
      </div>`
      });

Vue.component('appmodel', {
  props: ['is_example', 'is_link', 'is_selected', 'is_notselected', 'is_loading', 'is_model_loading', 'filename', 'filename1', 'filename2', 'model_info', 'progress_1', 'progress_2', 'progress_3', 'is_twoline'],
   methods: {
      onHandleDrop: function(event) { this.$emit('root_handle_drop',event); }
   },
data: function() {
    return { } },
    template: `<div class="block blockitem">
       <div v-if="is_loading=='true'">
       <appmodel_loading></appmodel_loading>
       </div>
       <div v-if="is_model_loading=='true'">
       <appmodel_model_loading></appmodel_model_loading>
       </div>
       <div v-if="is_example=='true'">
       <appmodel_choose v-on:change_model="$emit('change_choose')"></appmodel_choose>
       </div>
       <div v-if="is_link=='true'">
       <appmodel_link v-on:change_model="$emit('change_choose_url')"></appmodel_link>
       </div>
       <div v-if="is_selected=='true'">
       <appmodel_selected v-bind:progress_1="progress_1" v-bind:progress_2="progress_2" b-bind:progress_3="progress_3" v-bind:model_info="model_info" v-bind:filename="filename" v-bind:filename1="filename1" v-bind:filename2="filename2" v-bind:is_twoline="is_twoline" v-on:handle_drop="onHandleDrop"></appmodel_selected>
       </div>
       <div v-if="is_notselected=='true'">
       <appmodel_notselected v-on:examples_click="$emit('examples_click')"
       			     v-on:link_click="$emit('link_click')"
			     v-on:handle_drop="onHandleDrop"
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
.gltf, .glb, .stl, .obj, .mtl, .ds, dirs, .zip, .vox
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
  props: ['model_info','progress_1','progress_2','progress_3'],
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

   handle_changes : function()
   {
    this.$emit('change_model');
    this.$emit('change_category');
    
   },
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
	 this.state.appmodel_is_model_loading = "false";
	 this.state.appmodel_is_link = "false";
         if (val==0) this.state.appmodel_is_notselected = "true";
	 if (val==1) this.state.appmodel_is_selected = "true";
	 if (val==2) this.state.appmodel_is_examples = "true";
	 if (val==3) this.state.appmodel_is_loading = "true";
	 if (val==4) this.state.appmodel_is_link = "true";
	 if (val==5) this.state.appmodel_is_model_loading = "true";
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
	 this.change_model();
      },
      change_model3: function(selectfile) {
      if (repeat_prev==0) {
        repeat_prev=1;
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
var brg_db = "brightness_db.txt";
var mdl_db = "model_db.txt";


fetch(mat_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.material_db = str.split("\n");
   store.state.material_db.pop();
   });
});
fetch(bor_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.border_db = str.split("\n");
   store.state.border_db.pop();
   });
});
fetch(bck_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.background_db = str.split("\n");
   store.state.background_db.pop();
  
   });
});
fetch(brg_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.brightness_db = str.split("\n");
   store.state.brightness_db.pop();
  
   });
});
fetch(mdl_db).then(response => {
response.body.getReader().read().then(value=>{
   var str = strfy(value.value);
   store.state.model_db = str.split("\n");
   store.state.model_db.pop();
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

function parse_material_count(mat,type)
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
#canvas {
  background-image: url('load_spinner2.gif');
  background-size: 100% 100%;
</style>

<script>
function enable_spinner(a)
{
  //var el = document.getElementById("canvas2");
  var el2 = document.getElementById("canvas");
  if (a) {
    el2.style.backgroundImage = "url('load_spinner2.gif')";
    el2.style.backgroundSize = "100% 100%";
} else {
    el2.style.backgroundImage = "none";
    el2.style.backgroundColor = "#000000";
    el2.style.backgroundSize = "auto auto";
    
  }
}

function find_main_item(arr)
{
   var s = arr.length;
   for(var i=0;i<s;i++) {
      if (arr[i].substr(-4)==".zip") return i;
      if (arr[i].substr(-4)==".glb") return i;
      if (arr[i].substr(-4)==".vox") return i;
      if (arr[i].substr(-4)==".stl") return i;
      if (arr[i].substr(-5)==".gltf") return i;
      if (arr[i].substr(-3)==".ds") return i;
      if (arr[i].substr(-4)==".obj") return i;
      //if (arr[i].name.substr(-4)==".ply") return i;
   }
   console.log("ERROR: main item not found");
   set_label("ERROR: main item not found");
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
      model = "<?php echo $assetsite ?>/" + name;
   }

   //if (i==0) model="https://meshpage.org/assets/wooly_sheep.stl";
   //if (i==1) model="https://meshpage.org/assets/BoomBox.glb";
   //if (i==2) model="https://meshpage.org/assets/Duck.glb";
   //if (i==3) model="https://meshpage.org/assets/Astronaut.glb";
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
function get_is_animated()
{

   var s = contents_array2.length;
   for(var i=0;i<s;i++) {
      var filename = filename_array[i];
      if (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf") {
      var contents = contents_array2[i];
      var length = contents.length;
      var buffer = new ArrayBuffer( length );
      var view = new Uint8Array(buffer);
      for(var i=0;i<length;i++) { view[i] = contents[i]; }
      let str = new TextDecoder().decode(buffer);
      return str.includes("\"animations\"");
      }
  }
       
     // TODO, how to handle zip files.
  return false;
}

function get_border(i,m,filename,border_avoid)
{
  var color = "000000";
  var width = "0";
  if (i>=0 && i<store.state.border_db.length) {
     var name2 = store.state.border_db[i];
     color = parse_border_color(name2);
     width = parse_border_width(name2);
  }

  var anim_value = get_is_animated();

  var res = "";
  var variable = "I2";
  if (m==-1) variable="I1";
  
  if (width=="0") {
   //res+="P I5021=ev.polygon_api.recalculate_normals(" + variable + ");\n";
   res+="P I5023=ev.polygon_api.color(" + variable + ",0);\n"
   res+="ML I5022=ev.polygon_api.render_vertex_array_ml2(ev,I5023);\n"
  } else {

  res+= "P I205=ev.polygon_api.recalculate_normals(" + variable + ");\nP I206=ev.polygon_api.smooth_normals2(I205);\n"
  var five = "";
  if (/*anim_value &&*/ filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip"||filename.substr(-4)==".vox") { five="5"; if (border_avoid) return "ML I502=ev.mainloop_api.ml_empty();\n"; }
  if (filename.substr(-4)!=".vox") {
    res+= "MT I505=ev.materials_api.phong(ev,I" + five + "4,1.0,1.0,-400.0,ffff8844,ffffccaa,fffff8ee,30.0);\n";
    res+= "MT I504=ev.materials_api.acesfilm_material(ev,I505);\n";
    } else {
    res+="MT I505=ev.materials_api.mt_alt(ev,std::vector&lt;MT&gt;{I" + five + "4},0);\n";
    res+= "MT I504=ev.materials_api.acesfilm_material(ev,I505);\n"
    }
  //res+="MT I504=ev.materials_api.gltf_material(ev,I154,0,1,3.5,1.0,-400.0,400.0,400.0);\n";
  var gltf = ",false";
  if (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip") { gltf=",true"; }
  if (anim_value==true) { 
     //res+="MT I5011=ev.materials_api.gltf_anim_material2(ev,I154,0,30,I504,cvbnmdfghjklertyuiop,0);\n";
    res+= "MT I501=ev.materials_api.toon_border(ev,I504," + width + ",ff" + color + gltf + ");\n";
      } else {
    res+= "MT I501=ev.materials_api.toon_border(ev,I504," + width + ",ff" + color + gltf + ");\n";
      }

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
function parse_brg_value(brg)
{
  var arr = brg.split(" ");
  return arr[2];
}
function get_background_value()
{
  var elem = document.getElementById("background-select");
  return parseInt(elem.value);
}
function get_brightness_value()
{
  var elem = document.getElementById("brightness-select");
  return parseFloat(elem.value);
}
function get_brightness(i)
{
   var val = 1.0;
   if (i>=0 && i<store.state.brightness_db.length) {
      var name2 = store.state.brightness_db[i];
      var name = parse_brg_value(name2);
      val = name;
   }
   return val;
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
  var background="BM I41=ev.bitmap_api.newbitmap(100,100,ff" + color + ");\nBM I42=ev.bitmap_api.scale_bitmap_fullscreen(ev,I41);\nML I43=ev.sprite_api.vertex_array_render(ev,I42);\nMN I997=ev.move_api.mn_empty();\nMN I998=ev.move_api.scale2(I997,1.5,1.5,1.5);\nML I999=ev.move_api.move_ml(ev,I43,I998,1,10);\nML I44=ev.sprite_api.turn_to_2d(ev,I999,0.0,0.0,800.0,600.0);\n";
  return background;
}



function find_line_from_material_db(index)
{
  var s = store.state.material_db.length;
  for(var i=0;i<s;i++) {
     var elem2 = store.state.material_db[i];
     var cnt = parse_material_count(elem2);
     if (cnt==index) {
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
            hash = digit + (hash << 4);
        }

        var c = (hash & 0x00FFFFFF);
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
if (filename.substr(-4)==".glb"||filename.substr(-4)==".obj"||filename.substr(-4)==".zip"||filename.substr(-4)==".vox") { normals_val=2; } else { normals_val=0; }
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

 var metal = "MT I4=ev.materials_api.gltf_material3(ev," + roughstr + ",0.95,3.5,1.0," + rrs + "," + ggs + "," + bbs + ",1,1,-400.0,400.0,400.0);\n";

 var plastic = "MT I4=ev.materials_api.gltf_material3(ev," + roughstr +",0.1,3.5,1.0," + rrs + "," + ggs + "," + bbs + ",1,1,-400.0,400.0,400.0);\n";


var line = find_line_from_material_db(i);


if (parse_material_type(line)=='Metal') return [metal,normals_select];
if (parse_material_type(line)=='Plastic') return [plastic,normals_select];


var tex_url = parse_material_url(line);
var textured_material = "MT I4=ev.materials_api.gltf_material_from_file(ev," + tex_url + ",-400.0,400.0,400.0);\n";
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

  var brightness_value = get_brightness_value();
  var brightness = get_brightness(brightness_value);

  var border_value = get_border_value();

  var anim_value = true;

  var normals_val = get_normals_value();
  var border_color = "000000";
  var border_width = "1.0";
  var brd = get_border_value();
  var border_avoid = false;

  if (brd>=0 && brd<store.state.border_db.length) {
     var name2 = store.state.border_db[brd];
     border_color = parse_border_color(name2);
     border_width = parse_border_width(name2);
  }


  if (filename.substr(-4)==".vox") {
     res+="P I155=ev.voxel_api.vox_voxel3(ev," + filename + ",0,5,5,5);\n";
     res+="MT I627=ev.materials_api.colour_material(ev,0.5);\n"
     res+="MT I628=ev.materials_api.phong(ev,I627,30,-400,30,ff222222,ff888888,ffffffff,30);\n"
     res+="MT I629=ev.materials_api.acesfilm_material(ev,I628);\n"
     res+="ML I62=ev.voxel_api.vox_bind_ml2(ev," + filename + ",0,5,5,5,I629,0.0,ff000000);\n";
  } else
  if (filename.substr(-4)==".stl") { res+="P I17=ev.polygon_api.stl_load(" + filename + ");\nP I177=ev.polygon_api.fix_vertex_order(I17);\nP I18=ev.polygon_api.recalculate_normals(I177);\nP I191=ev.polygon_api.flip_normals(I18);\nP I19=ev.polygon_api.color_from_normals(I191);\nP I192=ev.polygon_api.flip_normals(I19);\nP I16=ev.polygon_api.fix_vertex_order(I192);\nP I155=ev.polygon_api.color_grayscale(I16);\n";
     } else
  if (filename.substr(-4)==".obj") {
     if (mtl_name=="") {
       res+="P I122=ev.polygon_api.p_url(ev," + filename + ",350);\n";
       res+="P I155=ev.polygon_api.fix_vertex_order(I122);\n";
       } else {
       res+="P I122=ev.polygon_api.p_mtl(ev," + filename +"," + mtl_name +"," + base_dir + ",600);\n";
       res+="P I155=ev.polygon_api.fix_vertex_order(I122);\n";
       }
     } else
  if (filename.substr(-3)==".ds") { res+="P I155=ev.polygon_api.p_url(ev," + filename + ",350);\n"; } else
  if (filename.substr(-4)==".zip") {
     res+="TF I186=ev.mainloop_api.gltf_load_sketchfab_zip("+filename+");\n"
     //es+="TF I186=ev.polygon_api.decimate_tf(I1860,0.5);\n";
     res+="P I172=ev.mainloop_api.gltf_mesh_all_p(ev,I186);\n";
     res+="P I155=ev.polygon_api.or_array3(std::vector<P>{I172});\n";
     if (normals_val!=3 && normals_val!=4)
     	{
	if (anim_value==true) {
        res+="ML I62=ev.mainloop_api.gltf_mesh_all_anim(ev,I186," + brightness + ",3.5,0.0,0,cvbnmdfghjklertyuiop,-400.0,400.0,400.0," + border_value + ",ff" + border_color + ",true);\n"; // 0.75
	border_avoid = true;
	} else {
        res+="ML I62=ev.mainloop_api.gltf_mesh_all(ev,I186," + brightness + ",3.5,0.0,0,-400.0,400.0,400.0," + border_value + ",ff" + border_color + ",true);\n"; // 0.75
	border_avoid=true;
	}
	}
  } else
  if (filename.substr(-4)==".glb") {
     res+="TF I186=ev.mainloop_api.gltf_loadKK("+base_dir+","+filename+");\n"
     //res+="TF I186=ev.polygon_api.decimate_tf(I1860,0.5);\n";
     res+="P I172=ev.mainloop_api.gltf_mesh_all_p(ev,I186);\n";
     res+="P I155=ev.polygon_api.or_array3(std::vector<P>{I172});\n";
     if (normals_val!=3 && normals_val!=4) {
	if (anim_value==true) {
     res+="ML I62=ev.mainloop_api.gltf_mesh_all_anim(ev,I186," + brightness + ",3.5,1.0,0,cvbnmdfghjklertyuiop,-400.0,400.0,400.0," + border_value + ",ff" + border_color + ",true);\n";
     border_avoid=true;
     } else {
     res+="ML I62=ev.mainloop_api.gltf_mesh_all(ev,I186," + brightness + ",3.5,1.0,0,-400.0,400.0,400.0," + border_value + ",ff" + border_color + ",true);\n";
     border_avoid=true;
     }
     }
  } else
  if (filename.substr(-5)==".gltf") {
    res+="TF I186=ev.mainloop_api.gltf_loadKK("+base_dir+","+filename+");\n"
     //res+="TF I186=ev.polygon_api.decimate_tf(I1860,0.5);\n";
     res+="P I172=ev.mainloop_api.gltf_mesh_all_p(ev,I186);\n";
     res+="P I155=ev.polygon_api.or_array3(std::vector<P>{I172});\n";
     if (normals_val!=3 && normals_val!=4) {
     if (anim_value==true) {
     res+="ML I62=ev.mainloop_api.gltf_mesh_all_anim(ev,I186," + brightness + ",3.5,1.0,0,cvbnmdfghjklertyuiop,-400.0,400.0,400.0," + border_value + ",ff" + border_color + ",true);\n";
     border_avoid=true;
     } else {
     res+="ML I62=ev.mainloop_api.gltf_mesh_all(ev,I186," + brightness + ",3.5,1.0,0,-400.0,400.0,400.0," + border_value + ",ff" + border_color + ",true);\n";
     border_avoid=true;
     }
     }
  } else
     {
	res+="P I155=ev.polygon_api.cube(-300,300,-300,300,-300,300);\n";
	}


  var border = get_border(border_value,material_value,filename,border_avoid);
var out = "I4";

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
  res+="ML I665=ev.polygon_api.line_to_cone3(ev,I433," + border_width/2 + ",5,I4,ff" + border_color + ");\n"
res+="ML I767=ev.mainloop_api.depthfunc(I665,3);\n";
res+="ML I62=ev.mainloop_api.array_ml(ev,std::vector<ML>{I767});\n"

  } else
  if (normals_val==4)
  { // wireframe with hidden surface removal

  res+= "MT I4=ev.materials_api.m_def(ev);\n"
   res+="P I1=ev.polygon_api.get_face_count(I155);\n";
res+="P I114=ev.lines_api.p_towards_normal(I1,0.02);\n";
res+="P I124=ev.lines_api.p_towards_normal(I1,-0.02);\n";
res+="LI I115=ev.lines_api.from_polygon(I114);\n";
res+="LI I116=ev.lines_api.from_polygon(I124);\n";

  res+="ML I135=ev.polygon_api.line_to_cone3(ev,I115," + border_width/2 +",5,I4,ff" + border_color +");\n"
  res+="ML I136=ev.polygon_api.line_to_cone3(ev,I116," + border_width/2 +",5,I4,ff" + border_color +");\n"

var color = "000000";
var bg = get_background_value();
if (bg>=0&&bg<store.state.background_db.length) {
  var name2 = store.state.background_db[bg];
  var name = parse_bg_colour(name2);
  color = name;
  }

res+="P I145=ev.polygon_api.color(I155,ff" + color + ");\n";
res+="MT I198=ev.materials_api.colour_material(ev,1.0);\n"
res+= "MT I199=ev.materials_api.acesfilm_material(ev,I198);\n"

res+="ML I156=ev.materials_api.bind(I145,I199);\n"

res+="ML I665=ev.mainloop_api.array_ml(ev,std::vector<ML>{I136,I135,I156});\n";
res+="ML I666=ev.mainloop_api.depthmask(I665,true);\n";
res+="ML I767=ev.mainloop_api.depthfunc(I666,3);\n";
res+="ML I62=ev.mainloop_api.array_ml(ev,std::vector<ML>{I767});\n"
  } else {
   res+="P I1=ev.polygon_api.get_face_count(I155);\n";

  if (material[1]!="") {
     res+=material[1];
  } else
  if (!((filename.substr(-4)==".obj"&&mtl_name!="")||filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip"||filename.substr(-4)==".vox")) {
      res+="P I2=ev.polygon_api.recalculate_normals(I1);\n";
      }
  res+="MT I3=ev.materials_api.m_def(ev);\n";
  if (material[0]!="") {
     res+="MT I54=ev.materials_api.m_def(ev);\n";
     res+=material[0];
     var gltf = ",false";
     if (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip") { gltf = ",true"; }
     res+="MT I46=ev.materials_api.toon_border(ev,I4," + border_width + ",ff" + border_color +gltf + ");\n";
     out = "I46";
  } else
  if (filename.substr(-4)==".obj"&&mtl_name!="") {
     res+="MT I54=ev.materials_api.m_def(ev);\n";
     res+="MT I53=ev.materials_api.texture_many2(ev,0.5);\n"
    res+= "MT I4=ev.materials_api.acesfilm_material(ev,I53);\n"
  } else
  if (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip") {
     var five="";
     if (anim_value==true) { five="5"; }
     res+="MT I" + five + "4=ev.materials_api.gltf_material(ev,I186,0,1,3.5,1.0,-400.0,400.0,400.0);\n";
    if (anim_value==true) { 
      res+="MT I55=ev.materials_api.gltf_anim_material2(ev,I186,0,30,I54,cvbnmdfghjklertyuiop,0);\n";
    res+= "MT I4=ev.materials_api.acesfilm_material(ev,I55);\n"
      }
  } else if (filename.substr(-4)==".vox")
  {
  res+="MT I54=ev.materials_api.colour_material(ev,0.5);\n";
  res+="MT I55=ev.materials_api.phong(ev,I1,30.0,-400.0,30.0,ff221100,ffff8800,ffffffff,30.0);\n";
    res+= "MT I4=ev.materials_api.acesfilm_material(ev,I55);\n"
  } else {
     res+="MT I54=ev.materials_api.vertex_phong(ev,I3,-0.3,0.3,1.0,ff888888,ffffffff,5.0,0.5);\n";
    res+= "MT I4=ev.materials_api.acesfilm_material(ev,I54);\n"
  }

 if ((!(filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip"||filename.substr(-4)==".vox"))||material_value!=-1) { 
    res+="ML I62=ev.materials_api.bind(I2," + out + ");\n";
    }

    res+="P I771=ev.polygon_api.cube(-300.0,300.0,-300.0,300.0,-300.0,300.0);\n";
    res+="MT I772=ev.mainloop_api.mainloop_material(ev,I62);\n";
    res+="ML I63=ev.materials_api.bind(I771,I772);\n";

    res+="ML I64=ev.mainloop_api.depthmask(I63,true);\n";
    res+="ML I6=ev.mainloop_api.depthfunc(I64,3);\n";
  if ((parseInt(material_value)==-1&&parseInt(border_value)==0) && (filename.substr(-4)==".glb"||filename.substr(-5)==".gltf"||filename.substr(-4)==".zip")) {


  }


  if (normals_val==5||normals_val==6) { // wireframe

res+="P I114=ev.lines_api.p_towards_normal(I1,0.02);\n";
res+="P I124=ev.lines_api.p_towards_normal(I1,-0.02);\n";
res+="LI I115=ev.lines_api.from_polygon(I114);\n";
res+="LI I116=ev.lines_api.from_polygon(I124);\n";

  res+="MT I733=ev.materials_api.m_def(ev);\n"
  res+="ML I135=ev.polygon_api.line_to_cone3(ev,I115," + border_width/2 +",5,I733,ff" + border_color +");\n"
  res+="ML I136=ev.polygon_api.line_to_cone3(ev,I116," + border_width/2 +",5,I733,ff" + border_color +");\n"

res+="ML I555=ev.mainloop_api.array_ml(ev,std::vector<ML>{I136,I135});\n";
res+="ML I502=ev.mainloop_api.depthfunc(I555,3);\n";


  } else {
      res+=border; // outputs I502
  }

  // I502
  res+="ML I767=ev.mainloop_api.array_ml(ev,std::vector<ML>{I6,I502});\n";
}

  if (filename.substr(-4)==".glb" || filename.substr(-5)==".gltf"||filename.substr(-4)==".zip") {
    res+="ML I67=ev.mainloop_api.android_resize(ev,I767,1.0);\n";
    res+="ML I88=ev.mainloop_api.async_gltf(I67,I186);\n";
    res+="ML I89=ev.mainloop_api.mouse_roll_zoom2(ev,I88);\n";
    res+="ML I800=ev.mainloop_api.touch_rotate(ev,I89,true,true,0.01,0.01);\n";
    res+="ML I8=ev.mainloop_api.disable_polygons(I800);\n";



  } else {
    res+="ML I67=ev.mainloop_api.android_resize(ev,I767,1.0);\n";
    res+="ML I68=ev.mainloop_api.mouse_roll_zoom2(ev,I67);\n";
    res+="ML I800=ev.mainloop_api.touch_rotate(ev,I68,true,true,0.01,0.01);\n";
    res+="ML I8=ev.mainloop_api.disable_polygons(I800);\n";



  }
  res+="ML I9=ev.mainloop_api.right_mouse_pan(ev,I8);\n";
  res+=background;

  res+="ML I14=ev.mainloop_api.or_elem_ml(ev,I44,I9);\n";
  var variable = "I14";
  if (anim_value==true) {
  res+="ML I15=ev.mainloop_api.send_key_at_time(I14,0.01,99);\n"
  res+="ML I165=ev.mainloop_api.send_key_at_time(I15,0.02,99);\n"
  variable="I165";
  }
  
 // 
//res+="ML I11=ev.mainloop_api.hires_ml(ev," + variable + ", 4096);\n";
  res+="ML I11=ev.mainloop_api.perspective(ev," + variable + ",80.0,10.1,60000.0);\n";
  res+="RUN I12=ev.blocker_api.game_window2(ev,I11,false,false,0.0,1000000.0);\n";


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
var contents_array2 = [];
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
   res = undo_base64(res);
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
   contents_array2 = [];
   filename_array = [];
   var s = file_array.length;
   var counter = 0;
   var enc = new TextDecoder("x-user-defined");
   for(var i = 0;i<s;i++) {
     var file = file_array[i];
     let fileReader = new FileReader();
     var f = (fileReader,i) => {
     fileReader.onload = () => {
        let fileContents = fileReader.result;
	var binary = '';
	var bytes = new Uint8Array( fileContents );

        binary = enc.decode(bytes);
	contents_array.push(binary);
	contents_array2.push(bytes);
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
   contents_array2 = [];
   filename_array = [];
   var s = file_array.length;
   var counter = 0;
   var enc = new TextDecoder("x-user-defined");
   for(var i = 0;i<s;i++) {
     var file = file_array[i];
     let fileReader = new FileReader();
     var f = (fileReader,i) => {
     fileReader.onload = () => {
        let fileContents = fileReader.result;
	var binary = '';
	var bytes = new Uint8Array( fileContents );
		binary = enc.decode(bytes);

	contents_array.push(binary);
	contents_array2.push(bytes);
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
   load_files(contents_array2,filename_array);
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
function load_finished2(succ)
{
   load_finished(1);
}
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
    promise.then(load_finished2);
    }
}

function drop3(state,selectfileelem)
{
   Module.ccall('set_string', null, ['number', 'string'],[6,""]);


   set_filename_info(state,"");
   var files2 = selectfileelem.target.files;

   set_model_info(state,"(loading..)");
   set_label("Loading model..");

  var files = [];
  var filenames = [];
  if (files2) {
     const promise2 = new Promise( resolve => {

     	   var s = files2.length;
     	   for(var i=0;i<s;i++) {
	   	   filenames.push(files2[i].name);
		   files.push(files2[i]);
	   }

           var main_item_num = find_main_item(filenames);
	   var main_item = files[main_item_num];
           var main_item_name = filenames[main_item_num];

	   main_item_name=do_base64(main_item_name);
	   filenames[main_item_num]=do_base64(filenames[main_item_num]);

	   old_files = files;
	   old_filenames = filenames;
	   old_main_item_name = main_item_name;

           const promise = extract_contents(state,files,filenames,fix_filename(main_item_name),"");
           promise.then(load_finished);

     });
     }


}

function drop(ev)
{
   //console.log("CLEARING CACHES");
   Module.ccall('set_string', null, ['number', 'string'],[6,""]);


  set_model_info(store.state,"(loading..)");
  set_label("Loading model..");

  var mod = document.getElementById("model-select");
  if (mod) mod.value="-1";

  ev.preventDefault();

  var files = [];
  var filenames = [];
  if (ev.dataTransfer.files) {
     const promise2 = new Promise( resolve => {

     	   var s = ev.dataTransfer.items.length;
	   var promises = [];
	   //console.log(s);
	   var fl = ev.dataTransfer.items;
	   var ff = ev.dataTransfer.files;
     	   for(var i=0;i<s;i++) {
	   	   var item = ev.dataTransfer.items[i].webkitGetAsEntry();		   

		   //console.log(ev.dataTransfer.items[i]);
       		   if (item) {
		   //console.log(item);
		   promises.push(traverseFileTree(item));
	  	   }
		   }
	    resolve(Promise.all(promises));
       });
       promise2.then( data => {
           const [files,filenames] = flatten_arrays(data);

	   var filenames2 = [];
	   var s = filenames.length;
	   //console.log(s);
	   for(var i=0;i<s;i++)
	   {
		//console.log(filenames[i]);
	      filenames2.push(filenames[i]);
	   }

           var main_item_num = find_main_item(filenames2);
	   var main_item = files[main_item_num];
           var main_item_name = filenames2[main_item_num];

	   main_item_name = do_base64(main_item_name);
	   filenames2[main_item_num]=do_base64(filenames2[main_item_num]);

	   old_files = files;
	   old_filenames = filenames2;

	   old_main_item_name = main_item_name;
	   
           const promise = extract_contents(store.state,files,filenames2,fix_filename(main_item_name),"");
           promise.then(load_finished);

     });
     }
}

</script>

<script>

var canv = document.getElementById("canvas");
var Module = {
    onStartup : function() { check_em2()(); },
canvas : canv,
   locateFile : (function(path) { return path+"?<?php echo filemtime("engine_highmem.js") ?>"; }),
   arguments : [ "--size", "800", "600", "--code", default_script(), "--homepage", "<?php echo $assetsite ?>/", "--href", window.location.href],
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

    var src = "engine_highmem.js?"+data2;
    var vstr = agent.substring(idx+8);
    var vnum = parseInt(vstr);

    if (firefox && vnum<=78)
	src="engine_nothreads.js?" + data2;
    else if (firefox) src="engine_nothreads_highmem.js?" + data2;
    if (mobile) src="engine_lowmem_nothreads.js?"+data2;
    if (!crossOriginIsolated && !mobile) src="engine_nothreads_highmem.js?" + data2;
    if (!g_emscripten_running) {
      enable_spinner(true);
      const dpr = window.devicePixelRatio || 2;

      if (filename=="") {
      Module.arguments = [ "--size", (800*dpr).toString(), (600*dpr).toString(), "--code", default_script(), "--homepage", "<?php echo $assetsite ?>/", "--href", window.location.href];
      } else {
      Module.arguments = [ "--size", (800*dpr).toString(), (600*dpr).toString(), "--code", convert_enter_to_at(create_script(filename,contents,filenames)), "--homepage", "<?php echo $assetsite ?>/", "--href", window.location.href];
      }

      var script = document.createElement("script");
      script.setAttribute("src", src);
      document.getElementsByTagName("head")[0].appendChild(script);
      check_if_emscripten_running();
      } else {
      // loading contents:

      var script = create_script(filename,contents,filenames);
      Module.ccall('set_string', null, ['number', 'string'],[0,script]);
      }
      setTimeout(function() { check_emscripten_ready(state) }, 1000);
}

function load_files(data_array2, filename_array)
{
  var s2 = data_array2.length;
  for(var i=0;i<s2;i++) {
    var data3 = data_array2[i];


    var filename = filename_array[i];
    //console.log(filename);
    var sz = data3.length;

    Module.ccall('set_string', null, ['number', 'string'], [1,filename], {async:false} );

    for(var s=0;s<sz;s+=40960) {
      var use_sz = 40960;
      if (s>=sz-40960) {
        use_sz=sz-s;
      }
    Module.ccall('set_integer', null, ['number', 'number'], [2,use_sz], {async:false} );

    var sli = data3.slice(s,s+use_sz);
    const uint8 = sli; 

    let pointer = Module._malloc( uint8.length );
    HEAP8.set( uint8, pointer );      
    Module.ccall('set_string', null, ['number', 'number'], [3,pointer], {async:false} );
    Module._free( pointer );
    }
    Module.ccall('set_string', null, ['number', 'string'], [4,""], {async:false} );

  }
}


var g_emscripten_running = false;
function check_em() {
    return function() {
	g_emscripten_running = true;
	enable_spinner(false);

//console.log("EMSCRIPTEN RUNNING");
	//resize_event(null);
	//load_file();
	load_data();
	app.handle_changes();
	if (loading_data==0) {
	app.change_appmodel(0);	   
	}
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

var g_first_time=true;
function check_em2_func() {
      	if (g_first_time) {
	   //console.log("EMSCRIPTEN_READY");
	   store.state.appmodel_is_loading="false";
	   store.state.appmodel_is_model_loading="true";
	   g_first_time=false;
	}
        if (g_download_done==true)
	{
	   //console.log("DOWNLOAD READY");
	   check_em()();
	   g_first_time = false;
	   store.state.appmodel_is_model_loading="false";
	} else {
	  setTimeout(check_em2_func,100);
	}
}
var g_timer=null;

function check_em2() {
   if (g_timer!=null) clearTimeout(g_timer);
   Module['onStartup'] = function() { return function() { } };
  // console.log("DOWNLOAD WAITING..");
   return function() {
      setTimeout(function() { check_em2_func(); },100);
   }
}

var g_ready_bit=0; // c++ side will change this
function check_emscripten_running()
{
    var canv = document.getElementById("canvas");
    if (Module) {
	Module['onStartup'] = check_em2();
	g_timer = setTimeout(function() { if (g_ready_bit==1) check_em2()(); },1000);
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
      store.state.appmodel_is_model_loading = "false";
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
    	 setTimeout(function() { check_emscripten_ready(state) }, 1000);
    } else
    if (value==1) { emscripten_ready_callback(state); }
    else {
    	 setTimeout(function() { check_emscripten_ready(state) }, 1000);
	 }
	 } else
	 {
    	 setTimeout(function() { check_emscripten_ready(state) }, 1000);
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
  const dpr = window.devicePixelRatio || 2;
  elem.width = (scale_x)*dpr;
  elem.height = (scale_y)*dpr;

}

function prettyStringifyObject(obj) {
  let result = ''
  for (const key in obj) {
    result = `${result}${result !== '' ? ', ' : ''}${key}: ${Array.isArray(obj[key]) ? `[${obj[key]}]` : obj[key]}`
  }
  return `{${result}}`
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
	brightness : get_brightness_value(),
         material : get_material_value(),
	 category : elem && elem.value,
	 metal : elem2 && elem2.value,
	 plastic : elem3 && elem3.value,
	 textured : elem4 && elem4.value
   };
   //console.log(prettyStringifyObject(ser));
   return json_stringify(ser);
}

function deserialize_state(txt)
{
  //console.log("STATE");
  var ser = json_parse(txt);
  var model = ser.model;
  var normals = ser.normals;
  var border = ser.border;
  var bg = ser.background;
  var brg = ser.brightness;
  var cat = ser.category;
  var metal = ser.metal;
  var plastic = ser.plastic;
  var textured = ser.textured;
  //var mat = ser.material;

  //console.log(ser + " " + model + " " + normals + " " + border + " " + bg + " " + cat + " " + metal + " " + plastic + " " + textured);

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
  var elem4 = document.getElementById("brightness-select");
  if (elem4) elem4.value = brg;
  var elem6 = document.getElementById("metal-type-select");
  if (elem6) elem6.value = metal;
  var elem7 = document.getElementById("plastic-type-select");
  if (elem7) elem7.value = plastic; else { /*console.log("PLASTIC ERROR");*/ }
  var elem8 = document.getElementById("textured-type-select");
  if (elem8) elem8.value = textured;
}

function load_data()
{
/*
   var st = document.getElementById("formstate2");
   var ca = document.getElementById("formcontentsarray2");
   var fa = document.getElementById("formfilenamearray2");
   var gf = document.getElementById("formgfilename2");
   var gp = document.getElementById("formgpath2");
   */
   var st = download_result["state"];
   var ca = download_result["contentsarray"];
   var fa = download_result["filenamearray"];
   var gf = download_result["gfilename"];
   var gp = download_result["gpath"];
   //console.log(st.length);
   //console.log(ca.length);
   //console.log(fa.length);
   //console.log(gf.length);
   //console.log(gp.length);
   if (st != "") {
    loading_data=1;
   var a_st = "";
   if (st !="" && st != undefined)
      a_st = st;
   var a_ca = "";
   //console.log("CONTENTSARRAY");
   //console.log(ca);
   if (ca !="" && ca != undefined)
      a_ca = json_parse(ca);
   //console.log(fa.textContent);
   //console.log("FILENAMEARRAY");
   var a_fa = "";
   if (fa !="" && fa != undefined)
      a_fa = json_parse(fa);
   //console.log(gf.textContent);
   //console.log("FILENAME");
   var a_gf = "";
   if (gf != "" && gf != undefined)
      a_gf = json_parse(gf);
   //console.log(gp.textContent);
  //  console.log("GPATH");
  var a_gp = "";
   if (gp != "" && gp != undefined) 
      a_gp = json_parse(gp);


  // console.log(a_st.length);
  // console.log(a_ca.length);
  // console.log(a_fa.length);
  // console.log(a_gf.length);
  // console.log(a_gp.length);


  //console.log(a_ca[0].length);

   deserialize_state(a_st);
   contents_array = base64_to_array(a_ca);
   contents_array2 = contents_array.map(c => c.split('').map(function(x) { return x.charCodeAt(0); }));
   filename_array = base64_to_array(a_fa);
   g_filename = a_gf;
   g_path = a_gp;


   old_files = contents_array;
   old_filenames = filename_array;
	   //console.log("Dold_files=");
	   //console.log(old_files);
	   //console.log("Dold_filenames=");
	   //console.log(old_filenames);

old_main_item_name = g_filename;
   old_main_item_dir = g_path;
   }
}



function do_base64(data)
{

	var s = data.length;
	var pos = -1;
	for(var i=0;i<s;i++) {
	  if (data[i]=='.') pos=i;
	}
	if (pos==-1) { return data; }
	var data2 = data.substring(0,pos);
	var data3 = data.substring(pos+1);
	return btoa(unescape(encodeURIComponent(data2))) + "." + data3;
}


function undo_base64(data)
{

	var s = data.length;
	var pos = -1;
	for(var i=0;i<s;i++) {
	  if (data[i]=='.') pos=i;
	}
	if (pos==-1) { return data; }
	var data2 = data.substring(0,pos);
	var data3 = data.substring(pos+1);
	try {
	    //console.log(data2);
	    data2 = decodeURIComponent(escape(atob(data2)));
	    } catch(dd)
	    {
	    }
	return data2 + "." + data3;
}


function encodeBase64(val) {
    let uint8Array = new TextEncoder().encode(val);
    let chunkSize = 0x8000; // 32 KB chunks
    let result = '';
    for (let i = 0; i < uint8Array.length; i += chunkSize) {
        let chunk = uint8Array.subarray(i, i + chunkSize);
        result += String.fromCharCode.apply(null, chunk);
    }
    return btoa(result);
}



function decodeBase64(encoded) {
    // Step 1: Decode Base64 to a binary string
    let binaryString = atob(encoded);
    
    // Step 2: Create a Uint8Array from the binary string
    let uint8Array = new Uint8Array(binaryString.length);
    for (let i = 0; i < binaryString.length; i++) {
        uint8Array[i] = binaryString.charCodeAt(i);
    }

    // Step 3: Decode the Uint8Array to a string using TextDecoder
    let decodedString = new TextDecoder().decode(uint8Array);
    
    return decodedString;
}

function chunkDecode(encodedVal) {
  // First decode from base64
  const decoded = atob(encodedVal);
  
  // Define chunk size for URL decoding
  const chunkSize = 80000;
  const chunks = [];
  
  // Split the decoded string into chunks
  for (let i = 0; i < decoded.length; i += chunkSize) {
    const chunk = decoded.slice(i, i + chunkSize);
    // Decode each chunk
    const decodedChunk = decodeURIComponent(escape(chunk));
    chunks.push(decodedChunk);
  }
  
  // Join the decoded chunks
  return chunks.join('');
}
/*
async function array_to_base64(arr) {
    const res = [];
    
    for (const val of arr) {
        // Convert string to Uint8Array
        const encoder = new TextEncoder();
        const uint8Array = encoder.encode(val);
        
        // Process in chunks
        const chunkSize = 80000;
        const chunks = [];
        
        for (let i = 0; i < uint8Array.length; i += chunkSize) {
            const chunk = uint8Array.slice(i, Math.min(i + chunkSize, uint8Array.length));
            const decoder = new TextDecoder();
            const chunkStr = decoder.decode(chunk);
            chunks.push(new Blob([encodeURIComponent(chunkStr)]));
        }
        
        // Combine chunks using Blob
        const fullBlob = new Blob(chunks);
        const fullEncoded = await fullBlob.text();
        
        // Convert to base64
        const base64 = btoa(unescape(fullEncoded));
        res.push(base64);
    }
    
    return res;
}
*/
function array_to_base64(arr)
{
   var res = [];
   for(var i=0;i<arr.length;i++)
   {
     var val = arr[i];
     const chunkSize = 80000;
     const chunks = [];
     for(let ii=0;ii<val.length;ii+=chunkSize) {
        const chunk = val.slice(ii,ii+chunkSize);
	const encodedChunk = encodeURIComponent(chunk);
	chunks.push(encodedChunk);
     }
     let fullEncoded = '';
     for( const chunk of chunks) {
        fullEncoded += chunk;
	}
     var buffer = btoa(unescape(fullEncoded));
     //var buffer = encodeBase64(val);
     res.push(buffer);
   }
   return res;

}

function base64_to_array(arr)
{
   var res = [];
   for(var i=0;i<arr.length;i++) {
     var val = arr[i];
     var res4 = decodeURIComponent(escape(atob(val)));
     //var res4 = decodeBase64(val);
     res.push(res4);
   }
   return res;
  
}

function hash(val)
{
	return ((val*10001) ^ 0x26522663).toString();
}

var g_url_id = 0;

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
   /*
	var name = "https://meshpage.org/viewdata/num.txt";
	fetch(name).then(response => {
	    response.body.getReader().read().then(value => {
	   //console.log(value);
	   var str = strfy(value.value);
	   //console.log(str);
	   var num = parseInt(str);
	   console.log(num);*/
	   //console.log("URLID:");
	   //  console.log(g_url_id);
	   prog.innerHTML = "<a href='<?php echo $site ?>/view.php?id=" + hash(g_url_id) + "'><?php echo $site ?>/view.php?id=" + hash(g_url_id) + "</a>";
/*	   }); });*/
	   
   }
}


function formsubmit()
{

  //var st3 = document.getElementById("formcontentsarray");
  var at3 = json_stringify(array_to_base64(contents_array));
  //var st4 = document.getElementById("formfilenamearray");
  var at4 = json_stringify(array_to_base64(filename_array));
  //var st5 = document.getElementById("formnum");

  var contents_length = at3.length;
  var filename_length = at4.length;

  var data = new FormData();
  data.append("num",-1);
  var xhr = new XMLHttpRequest();
  //xhr.responseType = 'arraybuffer';
  xhr.open('POST','<?php echo $site ?>/submit_contents.php', false);

  var submitprogress = 0;

  xhr.onload = function() {
  	     var resp = xhr.response;
	     //console.log(resp);
	     g_url_id = parseInt(resp,10);
	     //console.log(g_url_id);
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
  		   var st5_val = json_stringify(g_filename);
		   if (g_path=="") { g_path="./"; }
		   var st6_val = json_stringify(g_path);

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
		   xhr2.responseType = 'arraybuffer';
  		   xhr2.open('POST','<?php echo $site ?>/submit_contents.php', true);	
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
<!--script src="https://code.jquery.com/jquery-3.4.1.slim.min.js" integrity="sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n" crossorigin="anonymous"></script>
<script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js" integrity="sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo" crossorigin="anonymous"></script>
<script src="https://cdn.jsdelivr.net/npm/bootstrap@4.4.1/dist/js/bootstrap.min.js" integrity="sha384-wfSDF2E50Y2D1uUdj0O3uMBJnjuUD4Ih7YwaYd1iqfktj0Uod8GCExl3Og8ifwB6" crossorigin="anonymous"></script-->