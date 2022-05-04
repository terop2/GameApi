<?php

require_once("user.php");

page_header(3);
page_title("frequently asked questions","");
echo "<div class=\"emscripten\">";
?>
<h2>What is meshpage.org?</h2>

<ul>
<li>Meshpage is a mechanism to create 3d animations and sending them to your friends.
</ul>

<h2>How does the site work?</h2>
<ul>
<li>You download the builder tool
<li>Create your demonstration with the tool
<li>You get piece of c++-like code
<li>Then you can "create a mesh"
<li>After creation, you get URL link to the created product
</ul>
<h2>What are the advantages in your approach?</h2>
<ul>
<li>OpenGL based software-only solution, so no need to buy a camera
<li>File size is very small. The file size is like 30 lines of c++ code,
instead of megabytes of video files
<li>It's based on "designing" 3d animations, instead of "recording" those animations
<li>End result can be sent via email to other people
</ul>
<h2>What are the disadvantages?</h2>
<ul>
<li>Work amounts are higher than simply pressing "record" button in a camera.
<li>Designing 3d animations usually requires some planning
<li>The tool is slightly more complicated than we wanted
<li>Flexibility of the underlying software is always a problem
<li>End product sizes are pretty small, so you can't expect full length movie
    as the output of this process
<li>Different animation models are very limited -- basically just translate/rotate
<li>We don't have sound or music support at all, because it's fundamentally based on recording industry technology, and we're not experts in that area.
</ul>
<h2>How is this different from other user-generated-content sites?</h2>
<ul>
<li>There's isn't any issues with copyrights -- i.e. no DCMA kind of problems (this is necessary because it must work in EU)
<li>All content for the site must be created with the builder tool
<li>The tool can control what content is allowed to be included to the product, thus we can check against copyright infringements
<li>Bm_url and FI_load are the biggest features in this area.
<p>
<li>If we don't like some content, we can "hide" it from the front page, but
    the urls still continue to work
</ul>
<h2>How does meshpage.org get licenses to user's content?</h2>
<ul>
<li>User still needs to agree to license the content he created with the tool
to meshpage.org and associates
<li>This is similar to how Youtube handles it, i.e. when user presses "Save & Publish" button on the web page, we get a license to display the content on our web page - before that, it'll only be displayed for that same user who is posting the content
<li>This is basically "implicit licensing".
<li>But current situation is that users are spending much less time creating content than what it took to create the GameApi library and GameApi Builder tool, so it can still be considered exchange of licenses -- user can use the builder tool in exchange of giving license to the content created with the tool.
</ul>
<h2>What other problems are you seeing currently?</h2>
<ul>
<li>Large problem in today's technologies is something called "Effort calculation".
<li>Effort calculation is based on the premise that web site authors are eating significant amount of time from their user base, but cannot even say critical questions like:
  <ul>
  <li>how much time they're eating from their user base?
  <li>how many people are using their site?
  <li>how they can ensure that work amounts imposed to the user base are staying within acceptable range?
  <li>what actions are taken, if work amount explodes and people are doing too much work to improve the user-generated-content-site?
  </ul>
This kind of basic information is usually missing, and "Effort Calculation" is the key metric how it should be handled.
</ul>
<h2>How do bm_url and FI_load work?</h2>

You might notice that bm_url and FI_load are refusing to load some
urls which are not around the user's homepage.<p>

We have some technology included that checks against user's copyright
infringements, before posting the content to meshpage.org. This is
implemented in the bm_url and FI_load features. Basically we use user's
homepage as a center of user's world, and implement a domain
restriction which limits usage of content inside meshpage.org
animations. For example when it loads png image from some url, it
checks whether the user's homepage was in the same domain.<p>

The theory behind this check is that we should allow only content that
user published himself, instead of all the content in the world, and
thus domain where user's homepage is at, is good candidate for allowed
content. This cannot provide absolute guarantee against copyright
infringements, but it prevents certain kind of spread pattern where
user picks urls from all around the world and uses them as content
inside the animations. Since meshpage.org is publishing this content
in the web page, we try to check problems in the content.<p>

This domain restriction check is more cunning that you'd expect, since if
you use several pieces of content in an animation, they need to all come from
the same domain. I.e. you can't pick one url from one domain and next url from
completely different place. Expectation is that animations contain more than
one piece of content/sprites/bitmaps/fonts/etc.., and thus the domain restriction can pinpoint the location of the user's real homepage address.<p>

Other features than bm_url and FI_load cannot use external content at
all, so all content coming from other features are built competely
using GameApi Builder tool. This ensures pretty much that the user
created it himself, at least if copy-pasting CodeGen output code snippets
are not counted.<p>

Note that currently we do not have login system for end users
implemented, so the domain check is per individual animation. Once
login system gets improved, it would allow implementing the domain
check for per-user homepage, instead of per-animation homepage. This
would improve the system, but is not currently implemented.

<h2>What features have domain check like bm_url & FI_load</h2>
<ul>
<li>bm_url
<li>p_url
<li>p_mtl
<li>p_mtl_url
<li>gltf
<li>newfont
<li>fi_load / load_font
<li>load_song
</ul>
This means these can load asyncronously data from url's and that
loading is handled via domain restriction.

<h2>Why builder can't load textures via p_mtl?</h2>
<ul>
<li>it can, but you need to use p_mtl and m_texture_many_p together.
</ul>

<h2>What technologies are you using to provide the features of the site?</h2>
<ul>
<li>C++
<li>php
<li>OpenGL
<li>SDL2
<li>glew
<li>emscripten
<li>libfreetype
<li>stb_image
<li>openvr
<li>holoplay
<li>tiny_gltf
<li>(normal browser environment)
</ul>
In addition to these external stuff, we have internal development in the following modules:
<ul>
<li>GameApi
<li>GameApi Builder/editor
<li>web_page
<li>mesh-page
<li>iot_main
<li>parser2
</ul>
<h2>What kind of features are available in the base system?</h2>
<ul>
<li>OpenGL instancing
<li>3d mesh data structure
<li>triangles/quads/lines/cones/spheres/cubes/torus
<li>colours/normals/texture coordinates
<li>materials like snow/brashmeta/web/distance fields
<li>shading
<li>animations/movement/matrices
<li>skeletal animations
<li>vertex animations
<li>recursive shapes/trees
<li>waveforms
<li>volumes
<li>2d and 3d rendering
<li>sprites / bitmaps
<li>circles/rectangles/triangles in 2d
<li>fonts/text rendering
<li>etc..
</ul>
<h2>Why do tool menus not do anything?</h2>
<ul>
<li>you can link boxes whenever the labels match
<li>display menu from right mouse button is disabled until linking has been completely done from the left side of the boxes -- this is because left side is inputs, and right side is outputs, and all inputs need to be available for box to work correctly
</ul>
<h2>What keybindings are there in the tool?</h2>
<ul>
<li>ctrl-s saves the data to mod.txt
<li>'a' rotates the model in most dialogs under "display"
<li>'d' rotates the model in most dialogs under "display"
<li>delete (in box title bar) deletes the box
<li>esc exits the application
<li>esc (in blk_window) exits the full screen mode
</ul>
<h2>What tools you should try immediately?</h2>
<ul>
<li>m_snow, m_flat for shading
<li>anim_translate/anim_rotate for movement
<li>move_ml for movement
<li>seq_ml for moving from one scene to another
<li>cube/sphere/cone/torus for basic shapes
<li>p_render/m_bind for basic rendering
<li>p_render_inst/m_bind_inst for OpenGL instancing
<li>li_from_polygon/li_to_cone/random_quad for some amazing effects
<li>static_instancing/ms_static_inst for cpu side instancing
<li>blk_window and MainLoopApi features to finish your product
</ul>
<h2>I get error while loading the animations</h2>
<ul>
<li>With new pthreads support in the site, browser settings need to be changed
<li>Chrome users: chrome://flags should have webassembly pthreads enabled
<li>Firefox users: about:config shuold have shared.memory=true
</ul>
<h2>What browser should I use?</h2>
On my laptop I get the following benchmarks(this test: <a href="https://meshpage.org/mesh_display?id=GVILK@032">here</a>(
<ul>
<li>some users have reported 700fps, but with high-end video card
<li>Brave browser gives about 147fps
<li>firefox is 54fps
<li>Chrome is at 30fps
</ul>
<h2>What does blogs think about our site?</h2>
<ul>
<li>mediaisnothingtomebutistilllikeit <a href="https://mediaisnothingtomebutistilllikeit.wordpress.com/2017/12/06/meshpage-org/">blog</a>
</ul>
<?php
echo "</div>";
page_footer();