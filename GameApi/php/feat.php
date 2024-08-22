<html>
<h1>GameApi FeatureList</h1>

<h2>Sprites/Bitmaps</h2>
<ul>
<li>(bm_render/bm_2d) Sprites</h3>
<li>(BM) Bitmaps
<li>(new|new_bm|new_fb|new_bb) Creating bitmaps
<li>(load|save_png|save_raw) Loading/Saving bitmaps
<li>(bm_url|bm_fetch|2) Loading bitmaps from url
<li>(bm_gltf) Loading bitmap from glb/gltf files
<li>(bm_atlas_x|y|xy) Sprite atlas support
<li>(bm_cubemap|invcubemap) Cubemap support
<li>(edge_fix) Fix edges
<li>(border|add_shape_border) Add border to a bitmap
<li>(sub|alt|compose_x|y) subarea/alternatives/composition
<li>(blit|blitFB|blitBB) Blit the bitmaps to other bitmaps
<li>(bm_scale|bm_scale2) Scale the bitmap
<li>(bm_fullscreen) Fullscreen sized bitmap
<li>(interpolate) interpolate the bitmap
<li>(bm_color) Change color
<li>(bm_plus) Add colours together
<li>(bm_medianfilter) filter
<li>(repeat|flip_x|flip_y|rot90) Repeat and flip and rotate bitmaps
<li>(indicator|noise_vec|chessboard|mandelbrot) standard bitmaps
<li>(fractal_mountain) mountain bitmap
<li>(lightmap) lightmap calc
<li>(gradient|circ_gradient) Linear and circular gradient
<li>(blur) blur filter
<li>(floodfill|floodfill_color|floodfill_color_inv) Flood filling
<li>(gray_to_black) colour changes
<li>(bm_bumpmap) Bumpmapping
<li>(memoize) Caching
<li>(bb_from_bitmap) Bool bitmap from bitmap conversion
<li>(from_float_bitmap) Float bitmap to bool bitmap conversion
<li>(line|tri|circle|rectangle|part_circle|rings) Bitmap drawing primitives
<li>(andnot_bitmap|or_bitmap|xor_bitmap|choose_bitmap|bb_sprite) Boolean operations on bitmaps
<li>(to_bitmap) from bool bitmap to colour bitmap conversion
<li>(from_red|from_green|from_blue|from_alpha) Get float bitmap from colour channels
<li>(gradient_fb|radial_fb) gradients
<li>(sin_fb|plus_fb|mul_fb) calculating with colours
<li>(bm_dist|bm_blur|bm_gaussian|bm_perlin) standard bitmaps
<li>(to_grayscale_color) From float bitmap to coloured version
</ul>
<h2>3D Models</h2>
<ul>
  <li>(P) .obj, .mtl, .stl, .glb, .gltf files support
  <li>(triangle|quad|quad_x|quad_y|quad_z|quad_fullscreen) primitives
  <li>(polygon|cube|rounded_cube|sphere) standard primitives
  <li>(disc|cone|torus|bar_chart) standard primitives
  <li>(bg_image) background_image
  <li>(quad_bmsize) bitmap sized quad
  <li>(vr_fullscreen_quad) virtual reality fullscreen quad
<li>(heighmap) heightmaps
<li>(p_or_elem|array) combining 3d models
<li>(p_facecutter) cut faces
<li>(p_mix) combine
<li>(translate/rotatex|y|z/scale) translate/rotate/scale
<li>(subst) substitute
<li>(fix_vertex_order) fixing winding order of triangles
<li>(filter_invisible) filter out invisible triangles
<li>(line_to_cone|2|3) lines converted to cones
<li>(p_mix_color|p_min_color|p_max_color) colour mixing
<li>(color_darkness|color_gradient|color_distance|color_range) colour manipulation
<li>(recalc_normals) recalculating normals
<li>(smooth_normals) smooth normals
<li>(spherical_normals) spherical normals
<li>(flip_normals) flipping normals
<li>(texcoord) texture coordinates
<li>(p_tex_splitter|p_tex_storage|p_tex_light|p_tex_replace) splitting textures from 3d models
<li>(splitquads|x_split|y_split|z_split) splitting polygons
<li>(memoize|persistent_cache) polygon caching
</ul>
<h2>Materials</h2>
<ul>
<li>m_colour
<li>m_textures
<li>m_phong
<li>m_snow
<li>m_edge_glow
<li>m_toon_border
<li>m_gltf material
<li>m_fog
<li>m_dir_shadow
<li>m_or_elem combining materials
</ul>
<li>custom shader support
<h2>lines rendering</h2>
<ul>
<li>face edge lines
<li>translate/scale lines
</ul>
<h2>points rendering</h2>
<ul>
<li>loading points from file
<li>random points picked from quads
<li>random points picked from volumes
</ul>
<h2>minecraft rendering</h2>
<ul>
<li>default cubes
<li>default material
<li>voxels
</ul>
<h2>moving 3d models</h2>
<ul>
<li>translate/rotate/scale
<li>local move
<li>anim translate/rotate/scale
</ul>
<h2>voxels</h2>
<ul>
<li>(vx_or_elem) combining voxels
<li>(vx_blit) volume to voxel conversion
<li>(vx_inst) voxel to points conversion
<li>(vx_render) rendering voxels via cubes and points
<li>(vx_bind) rendering voxels with materials
<li>(vx_static) combining voxels to larger mesh
</ul>
<h2>volumes</h2>
<ul>
<li>sphere/cube/cone/torus
<li>colour
<li>move/rotate
<li>not/min/max/andnot
<li>(o_inst) instantiation
</ul>
<h2>fonts</h2>
<ul>
<li>FI_load
<li>FI_drawtext
<li>FI_largetext
</ul>
<h2>chooser tools</h2>
<ul>
<li>bm_chooser
<li>p_chooser
<li>ml_chooser
<li>ml_anim
</ul>
<h2>conditional execution</h2>
<ul>
<li>area
<li>toggle
<li>timed
<li>move
<li>repeat
<li>keypress
<li>statemachine
<li>hmd
</ul>
<h2>tree rendering</h2>
<ul>
<li>levels
<li>(tree_p) tree to mesh conversion
</ul>
<h2>textures</h2>
<ul>
<li>binding
<li>conversion to bitmap
<li>fbos
<li>(pbos)
</ul>
<h2>boolean operations on meshes</h2>
<ul>
<li>cube/sphere
<li>combining
<li>and_not
<li>intersect
</ul>
<h1>MainLoop features</h1>
<h2>Sequencing</h2>
<ul>
<li>(t_start|t_time|t_time_switch|t_exit) Sequencing effects
<li>(html_url/bm|p|ml|mn|mt_script) scripting support
<li>generating bitmaps via scripts
</ul>
<h2>gltf</h2>
<ul>
<li>(ml_gltf_all) gltf rendering
<li>(gltf_load2) fetching assets from web
<li>(async_gltf) async loading of assets
</ul>
<h2>main loop 3d modelling operations</h2>
<ul>
<li>(or_elem_ml|array_ml) combining mainloops
<li>(move_ml) moving 3d models
<li>(repeat_ml) repeating events
</ul>
<h2>rendering</h2>
<ul>
<li>(sp_render) sprite rendering
<li>(p_render|_inst|_txid) 3d model rendering
<li>(pts_render) points rendering
<li>(m_bind|_many|_inst|_instmat|_instmany) 3d model rendering with materials
<li>(b_divide|b_bind|b_draw) splitting 3d models to smaller pieces and rendering
</ul>
<h2>input handling</h2>
<ul>
<li>(wasd_ml|quake_ml|quake_ml2) input handling
<li>(key_activate|comb_key_activate) keyboard handling
<li>(touch_rotate) rotating with touchscreen/mouse
<li>(mouse_zoom|2) zooming with mouse wheel
<li>(mouse_pan) moving with right mouse button
<li>(key_record/key_playback) recording and playback of key events
</ul>
<h2>debugging</h2>
<ul>
<li>(key_printer_ml) debugging key events
<li>(mesh_stats_ml) debugging mesh properties
<li>(print_deps) printing internal dependencies
<li>(joy_printer_ml) joystick printer
</ul>
<h2>displaying panels and screens</h2>
<ul>
<li>(fps_display) fps display
<li>(score|time_display) displaying score and time
<li>(restart_key|restart_screen) displaying restart screen
</ul>
<h2>low level opengl</h2>
<ul>
<li>(ogl_|disable_polygons|depthfunc|depthmask|blendfunc|zbuf_disable|transparency|perspective) low level opengl operations
</ul>
<h2>internal</h2>
<ul>
<li>(concurrent_download) loading sanmiguel concurrently
<li>(eng_face_count) passing face count to javascript
</ul>
<h2>sounds</h2>
<ul>
<li>(song_ml) playing music via .ogg files
<li>(sound_ml) playing sound effects via .wav files
</ul>
<h2>window handling and deploying</h2>
<ul>
<li>(run_window) fullscreen window
<li>(html_run|html_ml|html_p|html_mt) converting engine to html5
<li>(looking_glass) holographic screen support (windows only)
<li>(vr_window) virtual reality window support
<li>(pkg_window|2) packing engine features to independent binary files
<li>(save_deploy) converting html5 to .zip file for deploying at hosting space
</ul>

<p><p><p><p>That's all folks<p><p><p><p><p>

</html>
